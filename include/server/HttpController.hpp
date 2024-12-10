#pragma once

#include <exception>
#include <functional>
#include <iostream>
#include  "HttpServer.hpp"

#define mv(X) std::move(X)
#define REG_ENDP(FUNCTION, NAME, TYPE) HttpController::register_method(TYPE, NAME, [this] (HttpRequest &&req, HttpResponse &&resp) { FUNCTION(mv(req), mv(resp)); })

class HttpController {
public:
    HttpController() {
        
    }
    //RequestType type, const std::string &endpoint_name, Callback foo
    template<typename... Values>
    static void register_method(Values... val) {
        HttpServer::instance().method_add(std::forward<Values>(val)...);
    }

};


