#pragma once
#include <server/HttpFilter.hpp>



class MyFilter2 : public HttpFilter {
public:
    MyFilter2() {
        printf("done reg\n");
        REG_FILT("/zov", doFilter);
    }
protected:
    bool doFilter(HttpRequest &&req) override {
        std::cout << "Filtering before...\n";
        if (!req.get_header("Goida").has_value()) {
            return false;
        }
        return true;
    }

};