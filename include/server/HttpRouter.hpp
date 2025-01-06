#pragma once

#include "server/HttpBinder.hpp"
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <algorithm>
#include <exception>
#include <functional>
#include <stdexcept>
#include "Utils.hpp"
#include "hash.hpp"
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