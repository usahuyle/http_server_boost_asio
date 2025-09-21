#pragma once
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <memory>
#include "router.hpp"

using boost::asio::ip::tcp;
using boost::asio::ssl::stream;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(stream<tcp::socket> socket, const Router& router);
    void start();

private:
    void do_handshake();
    void do_read();
    void do_write(const HttpResponse& response);
    HttpRequest parse_request(std::istream& request_stream);

    stream<tcp::socket> socket_;
    boost::asio::streambuf buffer_;
    const Router& router_;
};