#pragma once
#include <server/HttpFilter.hpp>



class MyFilter : public HttpFilter<MyFilter> {
public:
    MyFilter() {
        
        REG_FILT("/zov", doFilter);
    }
protected:
    bool doFilter(const HttpRequest &req) {
        std::cout << "Filtering before...\n";
        if (!req.get_header("Authorization").has_value()) {
            return false;
        }
        return true;
    }

};