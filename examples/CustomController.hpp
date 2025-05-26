#pragma once


#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <format>
#include <json/value.h>
#include <server/HttpRequest.hpp>





class MyController : public HttpController<MyController> {
public:
    MyController() {
        REG_ENDPOINT(reg, "/reg", RequestType::POST, RequestType::OPTIONS);
        REG_ENDPOINT(smth, "/auth/cock/fuck?age={age}", RequestType::GET, RequestType::OPTIONS);
    }
protected:
    void reg(const HttpRequest& req, HttpResponseWriter&& resp) {
        
        auto body = req.body_as_json();
        auto response = HttpResponseBuilder().set_type(ResponseType::JSON).set_body(*body).build();

        resp.respond(response);
    }

    void smth(const HttpRequest& req, HttpResponseWriter&& resp) {

    }
    
};