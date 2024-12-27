#pragma once
#include <server/HttpFilter.hpp>



class MyFilter2 : public HttpFilter<MyFilter2> {
public:
    MyFilter2() {
      
        REG_FILT("/zov", doFilter);
    }
protected:
    bool doFilter(const HttpRequest &req) {
        std::cout << "Filtering before...\n";
        if (!req.get_header("Goida").has_value()) {
            return false;
        }
        return true;
    }

};