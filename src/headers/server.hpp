#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include "router.hpp"

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, const Router& router);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    const Router& router_;
};