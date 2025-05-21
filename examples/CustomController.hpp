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
        REG_ENDPOINT(reg, "/smth/{id}?name={name}", RequestType::GET, RequestType::OPTIONS);
    }
protected:
    void reg(const HttpRequest& req, HttpResponseWriter&& resp) {
        
        auto a = req.get_query("id").as<long long>();

        std::cout << req.get_cookie("sessionId").has_value() << std::endl;
        // // std::cout << req.get_cookie("userId")->get_value() << std::endl;

        auto cookie = CookieBuilder()
        .set_domain("/")
        .set_httponly(true)
        .set_max_age(10000)
        .set_name("token")
        .set_value("sdjfjfjdsjklFJLKLjlkSE88J")
        .set_samesite(SameSite::Lax)
        .build();
        cookie.set_secure(true);

        auto response = HttpResponseBuilder().set_type(ResponseType::TEXT).set_body("fuck").build();
        response.add_cookie(cookie);

        resp.respond(response);
    }

    
};