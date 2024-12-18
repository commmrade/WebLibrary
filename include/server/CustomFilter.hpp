#pragma once
#include <server/HttpFilter.hpp>



class MyFilter : public HttpFilter {
public:
    MyFilter() {
        printf("done reg 1\n");
        REG_FILT("/zov", doFilter);
    }
protected:
    bool doFilter(const HttpRequest &req) override {
        std::cout << "Filtering before...\n";
        if (!req.get_header("Authorization").has_value()) {
            return false;
        }
        return true;
    }

};