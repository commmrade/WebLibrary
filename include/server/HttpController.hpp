#pragma once

#include<stdarg.h>
#include <utility>
#include "debug.hpp"
#include "server/HttpBinder.hpp"

#define mv(X) std::move(X)         
#define REG_ENDPOINT(FUNCTION, NAME, TYPE, ...) register_method(NAME, [this] (const HttpRequest &req, HttpResponse &resp) { FUNCTION(mv(req), mv(resp)); }, TYPE, __VA_ARGS__)


template<typename Derived>
class HttpController {
public:
    HttpController() {
        
    }

    HttpController(const HttpController&) = delete;
    HttpController(HttpController &&) = delete;
    HttpController& operator=(const HttpController&) = delete;
    HttpController& operator=(HttpController&&) = delete;


    //RequestType type, const std::string &endpoint_name, Callback foo, (Optional) Filter filter 
    template<typename... Values>
    static void register_method(Values... val) {
        debug::log_info("Registering a handler");
        HttpBinder::instance().register_handler(std::forward<Values>(val)...);
    }
};


