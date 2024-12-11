
#include <server/HttpFilter.hpp>



class MyFilter : public HttpFilter {
public:
    MyFilter() {
        REG_FILT("/zov", doFilter);
    }
protected:
    bool doFilter(HttpRequest &&req) override {
        std::cout << "Filtering before...\n";
        if (!req.get_header("Authorization").has_value()) {
            return false;
        }
        return true;
    }

};