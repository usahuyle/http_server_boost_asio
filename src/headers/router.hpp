#pragma once
#include <string>
#include <map>
#include <functional>

struct HttpResponse {
    std::string status_line;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpRequest {
    std::string method;
    std::string path;
    std::string version;
    std::map<std::string, std::string> headers;
};

class Router {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    void add_route(const std::string& path, Handler handler);
    HttpResponse route(const HttpRequest& req) const;

private:
    std::map<std::string, Handler> routes_;
};
