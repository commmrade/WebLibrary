#pragma once
#include "server/HttpResponse.hpp"
#include <limits>
#include <server/HttpFilter.hpp>



class MyFilter : public HttpFilter<MyFilter> {
public:
    MyFilter() {
        REG_FILTER("/smth/{id}?name={name}", do_filter);
    }
protected:
    bool do_filter(const HttpRequest &req) {
        req.add_header("XXX-origin-cors-shit", "ah");
        if (std::numeric_limits<int>::max() != 10) {
            return false;
        }

        return true;
    }

};