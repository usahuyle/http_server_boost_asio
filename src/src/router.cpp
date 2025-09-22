#include "../headers/router.hpp"
#include <map>

void Router::add_route(const std::string& path, Handler handler) {
    routes_[path] = handler;
}

HttpResponse Router::route(const HttpRequest& req) const {
    std::map<std::string, Handler>::const_iterator it = routes_.find(req.path);
    if (it != routes_.end()) {
        return it->second(req);
    }

    // Default: 404
    HttpResponse res;
    res.status_line = "HTTP/1.1 404 Not Found";
    res.body = "404 Not Found";
    res.headers["Content-Type"] = "text/plain";
    res.headers["Content-Length"] = std::to_string(res.body.size());

    std::map<std::string, std::string>::const_iterator connection = req.headers.find("Connection");
    if (connection != req.headers.end() && connection->second == "keep-alive"){
        res.headers["Connection"]= "keep-alive";
    }else{
        res.headers["Connection"]= "close";
    }
    res.headers["Keep-Alive"] = "timeout=60, max=1000";
    return res;
}
