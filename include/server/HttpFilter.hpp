#pragma once
#include "server/HttpRequest.hpp"
#include <server/HttpServer.hpp>



#define mv(X) std::move(X)
#define REG_FILT(NAME, FUNCTION) HttpFilter::register_filter(NAME, [this] (const HttpRequest &req) { return FUNCTION(mv(req)); }) // Macro to register filter

class HttpFilter {
public:
    HttpFilter() {
        
    }

    virtual bool doFilter(const HttpRequest &req) = 0;

    template<typename... Values>
    static void register_filter(Values... val) {
        HttpRouter::instance().register_filter(std::forward<Values>(val)...);
    }

};
