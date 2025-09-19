#include "../headers/server.hpp"
#include "../headers/session.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/system/detail/error_code.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <thread>

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, short port, const Router& router, int num_thread)
    : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), 
    router_(router), 
    io_context_(io_context), 
    num_thread_(num_thread) {

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

void Server::run(){
    for(size_t i=0; i < num_thread_; ++i){
        workers_thread_.emplace_back([this, i](){
            std::cout << "Worker thread " << i << " starting\n";
            io_context_.run();
            std::cout << "Worker thread " << i << " finished\n";
        });
    }
    io_context_.run();
    for (std::thread &t: workers_thread_){
        if(t.joinable()){
            t.join();
        }
    }
}