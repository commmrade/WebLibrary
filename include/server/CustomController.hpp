#include "server/HttpController.hpp"


class MyController : public HttpController {
public:
    MyController() {
    }

    BEGIN_METHODS
    METHOD_ADD("/zov", RequestType::GET, &MyController::reg);
    END_METHODS
    
protected:
    static void reg(HttpRequest&& req, HttpResponse&& resp) {
    
        auto login = req.get_query("name");
        auto password = req.get_query("password");
        
        if (login->empty() || password->empty()) {
            
            resp.set_header_raw("Content-Type", "text/plain");
            resp.write_str("ne works", 400);
            return;
        }

        
        resp.set_header_raw("Content-Type", "text/plain");
        resp.write_str("zov", 200);
    }

    
};