#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include<stdarg.h>
#include "HttpRouter.hpp"

#define mv(X) std::move(X)
#define REG_ENDP(FUNCTION, NAME, TYPE, ...) HttpController::register_method(TYPE, NAME, [this] (HttpRequest &&req, HttpResponse &&resp) { FUNCTION(mv(req), mv(resp)); }, ##__VA_ARGS__)

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


