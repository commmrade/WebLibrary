#pragma once

#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/Utils.hpp"





class MyController : public HttpController {
public:
    MyController() {
        REG_ENDP(reg, "/zov", RequestType::GET);
        
    }

protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        


        Response rsp;
        rsp.add_header(HeaderType::AUTH_BEARER, "fdjdfj28djsahd27y");
        rsp.set_body("fuck you");
        rsp.set_status(201);
        rsp.set_version("2.0");
        
        
        resp.respond(rsp);
    }

    
};
