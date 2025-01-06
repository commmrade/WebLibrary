#pragma once


#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include <json/value.h>






class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDP(reg, "/smth/{id}?name={name}", RequestType::GET, RequestType::OPTIONS);
        
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        
        std::cout << req.get_query("id").as<long long>() << std::endl;
    
        Response r{200, "ok"};
        resp.respond(r);
    }

    
};
