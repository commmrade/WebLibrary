#pragma once

#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/Utils.hpp"





class MyController : public HttpController {
public:
    MyController() {
        REG_ENDP(reg, "/zov", RequestType::GET, RequestType::OPTIONS);
        
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        


        Response rsp{ResponseType::TEXT};
        rsp.add_header(HeaderType::AUTH_BEARER, "fdjdfj28djsahd27y");
        rsp.set_body("fuck you");
        rsp.set_status(201);
    
        resp.respond(rsp);
    }

    
};
