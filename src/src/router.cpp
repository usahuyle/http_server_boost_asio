#include "router.hpp"

void Router::add_route(const std::string& path, Handler handler) {
    routes_[path] = handler;
}

HttpResponse Router::route(const std::string& path) const {
    auto it = routes_.find(path);
    if (it != routes_.end()) {
        return it->second();
    }

    // Default: 404
    HttpResponse res;
    res.status_line = "HTTP/1.1 404 Not Found";
    res.body = "404 Not Found";
    res.headers["Content-Type"] = "text/plain";
    res.headers["Content-Length"] = std::to_string(res.body.size());
    res.headers["Connection"] = "close";
    return res;
}
