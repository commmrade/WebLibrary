#pragma once

#include<stdarg.h>
#include "HttpRouter.hpp"
#include "debug.hpp"

#define mv(X) std::move(X)             // More types (TODO: Special filters for 1 endpoint)
#define REG_ENDP(FUNCTION, NAME, TYPE, ...) register_method(NAME, [this] (const HttpRequest &req, HttpResponse &resp) { FUNCTION(mv(req), mv(resp)); }, TYPE, ##__VA_ARGS__)
// Macro to reg endpoint

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
        HttpRouter::instance().register_handler(std::forward<Values>(val)...);
    }
};


