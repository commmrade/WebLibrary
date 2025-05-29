#pragma once
#include <debug.hpp>
#include <tuple>
#include <expected>

enum class RequestType;
class HttpResponseWriter;
class HttpRouter {
public:
    HttpRouter() = default;

    HttpRouter(const HttpRouter&) = delete;
    HttpRouter(HttpRouter &&) = delete;
    HttpRouter& operator=(const HttpRouter&) = delete;
    HttpRouter& operator=(HttpRouter&&) = delete;    

    static HttpRouter& instance() { // Singleton
        static HttpRouter router{};

        return router;
    }
    void process_request(int client_socket, std::string_view call); // Processes string and etc
private:
    std::expected<std::pair<std::string, std::string>, std::string> parse_request_line(std::string_view request_string);
    void handle_request(HttpResponseWriter& resp, std::string_view path, std::string_view request_string, std::string_view method, RequestType request_type); // Calls handlers and filters
};