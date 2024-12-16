#pragma once
#include "server/HttpRequest.hpp"
#include <server/HttpServer.hpp>



#define mv(X) std::move(X)
#define REG_FILT(NAME, FUNCTION) HttpFilter::register_filter(NAME, [this] (HttpRequest &&req) { return FUNCTION(mv(req)); })

class HttpFilter {
public:
    HttpFilter() {
        
    }

    virtual bool doFilter(HttpRequest &&req) = 0;

    //str endp_name, function filter
    template<typename... Values>
    static void register_filter(Values... val) {
        HttpRouter::instance().register_filter(std::forward<Values>(val)...);
    }

};
