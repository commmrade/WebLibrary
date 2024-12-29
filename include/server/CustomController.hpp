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
        REG_ENDP(reg, "/zov", RequestType::GET, RequestType::OPTIONS);
        
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        

        std::ifstream file("index.html");

        std::stringstream file_buf;
        file_buf << file.rdbuf();
        
       
        Response rsp{200, file_buf.str(), ResponseType::HTML}; 
        
        resp.respond(rsp);
    }

    
};
