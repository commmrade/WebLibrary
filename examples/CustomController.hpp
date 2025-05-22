#pragma once


#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <format>
#include <json/value.h>






class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDPOINT(reg, "/smth", RequestType::POST, RequestType::OPTIONS);
    }
protected:
    void reg(const HttpRequest& req, HttpResponseWriter&& resp) {
        
        auto body = req.body_as_json();
        std::cout << body->toStyledString() << std::endl;
        std::cout << body->get("id", 0).as<int>() << std::endl;

        auto response = HttpResponseBuilder().set_type(ResponseType::JSON).set_body(*body).build();

        resp.respond(response);
    }

    
};