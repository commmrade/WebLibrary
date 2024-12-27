#pragma once

#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/Utils.hpp"
#include <json/value.h>





class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDP(reg, "/zov", RequestType::POST, RequestType::OPTIONS);
        
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        
       
        Response rsp{ResponseType::JSON};    
        rsp.set_status(200);
        Json::Value val;
        val["name"] = "dds";
        val["password"] = "26062006";
        rsp.set_body("dsksdj");
        resp.respond(rsp);
    }

    
};
