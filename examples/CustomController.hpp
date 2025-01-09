#pragma once


#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <format>
#include <json/value.h>






class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDPOINT(reg, "/smth/{id}?name={name}", RequestType::GET, RequestType::OPTIONS);
    }
protected:
    void reg(const HttpRequest& req, HttpResponse&& resp) {
        
        std::cout << req.get_query("id").as<long long>() << std::endl;
        auto a = req.get_cookie("aaa");
    
        auto response = ResponseBuilder().serve_file("static/public/fuck.css").build();
        resp.respond(response);
    }

    
};
