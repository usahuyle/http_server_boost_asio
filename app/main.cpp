#include "../src//headers/server.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <iostream>

int main() {
    try {
        boost::asio::io_context io_context;

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

        Server s(io_context, 8080, router);
        io_context.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}