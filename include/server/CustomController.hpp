#pragma once

#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/HttpServer.hpp"




class MyController : public HttpController {
public:
    MyController() {
        REG_ENDP(reg, "/zov", RequestType::GET);
        REG_ENDP(echo, "/echo?text=?", RequestType::GET);
    }

protected:
    void reg(HttpRequest&& req, HttpResponse&& resp) {
        auto login = req.get_query("name");
        auto password = req.get_query("password");
        
     

        resp.set_header_raw("Content-Type", "text/plain");
        resp.write_str("zov", 200);
    }

    void echo(HttpRequest &&req, HttpResponse &&resp) {
        auto str = req.get_query("text").value_or("NULL");


        resp.set_header_raw("Content-Type", "text/plain");
        resp.write_str(str, 200);
    }
};
