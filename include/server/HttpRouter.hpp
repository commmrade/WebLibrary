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
    void process_endpoint(int client_socket, const std::string &call);
private:
    bool is_file_url(const std::string &base_url);
};