#include "../src//headers/server.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>
#include <iostream>
#include <thread>

using boost::asio::ssl::context;


int main() {
    try {
        boost::asio::io_context io_context;
        context ssl_context(context::tlsv12_server);
        ssl_context.set_options(
            boost::asio::ssl::context::default_workarounds
          | boost::asio::ssl::context::no_sslv2
          | boost::asio::ssl::context::single_dh_use);

        ssl_context.use_certificate_chain_file("../../server.crt");
        ssl_context.use_private_key_file("../../server.key", context::pem);

        Router router;
        router.add_route("/", [] {
            HttpResponse res;
            res.status_line = "HTTP/1.1 200 OK";
            res.body = "Hello from HTTP";
            res.headers["Content-Type"] = "text/plain";
            res.headers["Content-Length"] = std::to_string(res.body.size());
            res.headers["Connection"] = "close";
            return res;
        });

        router.add_route("/time", [] {
            std::time_t now = std::time(nullptr);
            std::string body = std::string("Current time: ") + std::ctime(&now);
            HttpResponse res;
            res.status_line = "HTTP/1.1 200 OK";
            res.body = body;
            res.headers["Content-Type"] = "text/plain";
            res.headers["Content-Length"] = std::to_string(res.body.size());
            res.headers["Connection"] = "close";
            return res;
        });

        int num_thread= std::thread::hardware_concurrency();

        Server s(io_context, 8080, ssl_context ,router, num_thread);
        s.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}