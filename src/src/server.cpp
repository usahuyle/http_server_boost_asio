#include "../headers/server.hpp"
#include "../headers/session.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, short port, 
    boost::asio::ssl::context& ssl_context
    ,const Router& router, int num_thread)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), 
    ssl_context_(ssl_context),
    router_(router), 
    io_context_(io_context), 
    num_thread_(num_thread) {

    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
           try{
                //construct ssl socket from accepted socket in SSL stream context
                boost::asio::ssl::stream<tcp::socket> ssl_socket = boost::asio::ssl::stream<tcp::socket>(std::move(socket), ssl_context_);
                std::make_shared<Session>(std::move(ssl_socket), router_)->start();
            }catch(boost::system::error_code& ec){
                std::cerr << "Do Accept Error: " << ec.message() << "\n";
            }

            do_accept(); // keep accepting
        });
}

void Server::run(){
    for(size_t i=0; i < num_thread_; ++i){
        workers_thread_.emplace_back([this, i](){
            io_context_.run();
            std::cout << "Worker thread " << i << "\n";
        });
    }
    for (std::thread &t: workers_thread_){
        if(t.joinable()){
            t.join();
        }
    }
}