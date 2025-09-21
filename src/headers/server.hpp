#pragma once
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <thread>
#include <vector>
#include "router.hpp"

class Server {
public:
    Server(boost::asio::io_context& io_context, short port, boost::asio::ssl::context& ssl_context,
         const Router& router, int num_thread);
    void run();

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ssl::context& ssl_context_;
    const Router& router_;
    boost::asio::io_context& io_context_;
    int num_thread_;
    std::vector<std::thread> workers_thread_;
};