#pragma once

#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/Utils.hpp"
#include <fstream>
#include <json/value.h>
#include <sstream>





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
