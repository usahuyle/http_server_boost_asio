#pragma once
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <memory>
#include "router.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, const Router& router);
    void start();

private:
    void do_read();
    void do_write(const HttpResponse& response);

    HttpRequest parse_request(std::istream& request_stream);

    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf buffer_;
    const Router& router_;
};