#pragma once

#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"





class MyController : public HttpController {
public:
    MyController() {
        REG_ENDP(reg, "/zov", RequestType::GET);
        
    }

protected:
    void reg(HttpRequest&& req, HttpResponse&& resp) {
        auto login = req.get_query("name");
        auto password = req.get_query("password");
        
      


        Response rsp;
        rsp.add_header(HeaderType::AUTH_BEARER, "fdjdfj28djsahd27y");
        rsp.set_body("fuck you");
        rsp.set_status(201);

        resp.respond(rsp);
    }

    
};
