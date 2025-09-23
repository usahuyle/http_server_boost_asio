#include "../src//headers/server.hpp"
#include "router.hpp"
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

        // Enable TLS session resumption (server cache)
        SSL_CTX* native_ctx = ssl_context.native_handle();
            
        // Use built-in server cache
        SSL_CTX_set_session_cache_mode(native_ctx, SSL_SESS_CACHE_SERVER);

        // Set a unique session id context
        SSL_CTX_set_session_id_context(native_ctx,
            reinterpret_cast<const unsigned char*>("MyServerID"),
            sizeof("MyServerID") - 1);

        // Enable session tickets for TLS 1.3+
        SSL_CTX_set_options(native_ctx, SSL_OP_NO_TICKET);

        Router router;
        router.add_route("/", [](const HttpRequest& req) {
            HttpResponse res;
            res.status_line = "HTTP/1.1 200 OK";
            res.body = "Test Home Page";
            res.headers["Content-Type"] = "text/plain";
            res.headers["Content-Length"] = std::to_string(res.body.size());
            auto connection = req.headers.find("Connection");
            res.headers["Connection"]= (connection != req.headers.end() && connection->second == "keep-alive") 
                                        ? "keep-alive" 
                                        : "close";
            return res;
        });

        router.add_route("/time", [](const HttpRequest& req) {
            std::time_t now = std::time(nullptr);
            std::string body = std::string("Current time: ") + std::ctime(&now);
            HttpResponse res;
            res.status_line = "HTTP/1.1 200 OK";
            res.body = body;
            res.headers["Content-Type"] = "text/plain";
            res.headers["Content-Length"] = std::to_string(res.body.size());
            auto connection = req.headers.find("Connection");
            res.headers["Connection"]= (connection != req.headers.end() && connection->second == "keep-alive") 
                                        ? "keep-alive"  
                                        : "close";
            return res;
        });

        int num_thread= std::thread::hardware_concurrency();

        Server s(io_context, 8080, ssl_context ,router, num_thread);
        s.run();

    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}