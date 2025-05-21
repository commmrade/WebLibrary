#pragma once
#include <server/HttpFilter.hpp>



class MyFilter : public HttpFilter<MyFilter> {
public:
    MyFilter() {
        REG_FILTER("/smth/{id}?name={name}", doFilter);
    }
protected:
    bool doFilter(const HttpRequest &req) {
        std::cout << "Filtering before...\n";
        req.add_header("XXX-origin-cors-shit", "ah");

        if (false) {
            return false;
        }

        return true;
    }

};