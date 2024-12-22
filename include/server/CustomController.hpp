#pragma once

#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/Utils.hpp"





class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDP(reg, "/zov", RequestType::GET, RequestType::OPTIONS);
        
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        
        Response rsp{ResponseType::TEXT};
        rsp.add_header(HeaderType::AUTH_BEARER, "fdjdfj28djsahd27y");
        
        Cookie cookie{"Auth", "fuckdfdj"};
        cookie.set_httponly(true);
        cookie.set_secure(true);
        cookie.set_max_age(64000);
        cookie.set_path("/");
        cookie.set_httponly("localhost.com");
        cookie.set_samesite(SameSite::Lax);
        
        rsp.set_body("fuck you");

        rsp.set_status(201);
        rsp.add_cookie(cookie);
        resp.respond(rsp);
    }

    
};
