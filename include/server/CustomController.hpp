#pragma once


#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include <json/value.h>

class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDP(reg, "/zov?name={name}", RequestType::GET, RequestType::OPTIONS);
        
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
      
        Response rsp{200, req.get_query("name").value(), ResponseType::HTML}; 
        
        resp.respond(rsp);
    }

    
};
