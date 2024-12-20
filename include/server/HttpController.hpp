#pragma once

#include<stdarg.h>
#include "HttpRouter.hpp"

#define mv(X) std::move(X)             // More types
#define REG_ENDP(FUNCTION, NAME, TYPE, ...) HttpController::register_method(NAME, [this] (const HttpRequest &req, HttpResponse &resp) { FUNCTION(mv(req), mv(resp)); }, TYPE, ##__VA_ARGS__)
// Macro to reg endpoint


class HttpController {
public:
    HttpController() {
        
    }
    //RequestType type, const std::string &endpoint_name, Callback foo, (Optional) Filter filter 
    template<typename... Values>
    static void register_method(Values... val) {
        HttpRouter::instance().register_handler(std::forward<Values>(val)...);
    }

};


