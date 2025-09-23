#include "../headers/session.hpp"
#include "../headers/router.hpp"
#include <boost/asio/error.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/asio/write.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/asio/ssl.hpp>
#include <cstddef>
#include <iostream>
#include <istream>
#include <memory>
#include <sstream>
#include <map>
#include <string>
#include <thread>

using boost::asio::ip::tcp;
using boost::asio::ssl::stream;

Session::Session(stream<tcp::socket> socket, const Router& router)
    : socket_(std::move(socket)), router_(router) {}

void Session::start(){
    do_handshake();
}

void Session::do_handshake(){
    std::shared_ptr<Session> self(shared_from_this());
    socket_.async_handshake(boost::asio::ssl::stream_base::server,
    [this, self](boost::system::error_code ec){
        try{
            do_read();
        }catch(boost::system::error_code& ec){
            std::cerr << "TLS Handshake error: " << ec.message() << "\n";
        }
    });
}


void Session::do_read(){
    std::shared_ptr<Session> self(shared_from_this());
    boost::asio::async_read_until(socket_, buffer_, "\r\n\r\n",
    [this, self](boost::system::error_code ec, std::size_t Length){
        if(ec){
            if (ec == boost::asio::ssl::error::stream_truncated ||
                ec == boost::asio::error::eof){
                return;
            }
            std::cerr << "Read Error: "<< ec.message() << "\n";
            return;
        }
        std::istream request_stream(&buffer_);
        HttpRequest req = parse_request(request_stream);
        HttpResponse res = router_.route(req);
        do_write(res);
        }
    );
}

void Session::do_write(const HttpResponse& res) {
    std::shared_ptr<Session> self(shared_from_this());
    std::ostringstream response;
    response << res.status_line << "\r\n";
    for(auto &pair: res.headers){
        response << pair.first << ": " << pair.second << "\r\n";
    }
    response << "\r\n";
    response << res.body;
    std::string response_string = response.str();
    boost::asio::async_write(socket_, boost::asio::buffer(response_string), 
    [this, self, res](boost::system::error_code ec, std::size_t length){
        if(ec){
            if (ec == boost::asio::error::broken_pipe ||  
                ec == boost::asio::error::eof ||
                ec == boost::asio::error::connection_reset||
                ec == boost::asio::ssl::error::stream_truncated){
                    return;
                }
            std::cerr << "Write Error: " << ec.message() << "\n";
            boost::system::error_code shutdown_ec;
            socket_.shutdown(shutdown_ec);
            return;
        }

        auto it = res.headers.find("Connection");
        if (it != res.headers.end() && it->second == "keep-alive"){
            do_read();
        }else{
            boost::system::error_code shutdown_ec;
            socket_.shutdown(shutdown_ec);
        }
    });
}

HttpRequest Session::parse_request(std::istream& stream){
    std::string request_line;
    HttpRequest req;
    if (!std::getline(stream, request_line)) return req; // nothing to parse

    // remove trailing '\r' if present
    if (!request_line.empty() && request_line.back() == '\r') {
        request_line.pop_back();
    }

    std::istringstream rl(request_line);
    rl >> req.method >> req.path >> req.version;
    // std::cout << "Request Line: " << req.method << " " << req.path << " " << req.version << "\n";

    //parse headers
    std::string line;
    while (std::getline(stream, line) && line != "\r"){
        auto colon = line.find(':');
        if (colon != std::string::npos){
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon+1);

            if (!value.empty() && value.front() ==' '){
                value.erase(0,1);
            }
            if (!value.empty() && value.back() == '\r'){
                value.pop_back();
            }
            req.headers[key]= value;
        }
    }
    return req;
}