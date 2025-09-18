#include "../headers/server.hpp"
#include "../headers/session.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <memory>

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, short port, const Router& router)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), router_(router) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept(
        [this](boost::system::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), router_)->start();
            }
            do_accept(); // keep accepting
        });
}