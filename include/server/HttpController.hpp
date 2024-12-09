#pragma once

#include <exception>
#include <iostream>
#include  "HttpServer.hpp"


#define BEGIN_METHODS   static void init_path_routing() \
    {
#define METHOD_ADD(endpoint_name, type, callback) HttpController::register_endpoint(endpoint_name, type, callback)
    
#define END_METHODS return; \
    }


class HttpController {
public:
    HttpController() {
        
    }

    static void register_endpoint(const std::string &endpoint_name, RequestType type, Callback callback) {
        
        auto& app = HttpServer::instance();
        if (app.is_ran()) {
            throw std::runtime_error("Server is already listening! P.S: Add register endpoints before starting");
        }
        app.method_add(type, endpoint_name, callback);
    }
};


