#pragma once


#include <debug.hpp>

class HttpRouter {
public:
    HttpRouter() {

    }

    HttpRouter(const HttpRouter&) = delete;
    HttpRouter(HttpRouter &&) = delete;
    HttpRouter& operator=(const HttpRouter&) = delete;
    HttpRouter& operator=(HttpRouter&&) = delete;    

    static HttpRouter& instance() { // Singleton
        static HttpRouter router{};

        return router;
    }
    void process_endpoint(int client_socket, std::string_view call);
private:
    bool is_file_url(std::string_view base_url);
};