#pragma once


#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <format>
#include <json/value.h>
#include <ostream>
#include <server/HttpRequest.hpp>





class MyController : public HttpController<MyController> {
public:
    MyController() {
        // REG_ENDPOINT(reg, "/reg", RequestType::POST, RequestType::OPTIONS);
        REG_ENDPOINT(smth, "/auth/cock/fuck?age={age}", RequestType::GET, RequestType::OPTIONS);
        REG_ENDPOINT(smth2, "/some/{id}/{id2}?name={name}&surname={surname}", RequestType::GET, RequestType::OPTIONS);
    }
protected:
    void reg(const HttpRequest& req, HttpResponseWriter&& resp) {
        
        auto body = req.body_as_json();
        auto response = HttpResponseBuilder().set_type(ContentType::JSON).set_body(*body).build();

        resp.respond(response);
    }

    void smth([[maybe_unused]] const HttpRequest& req, HttpResponseWriter&& resp) {
        std::cout << "Called\n";
        auto response = HttpResponseBuilder().set_type(ContentType::TEXT).set_body("cock").build();
        resp.respond(response);
    }
    void smth2(const HttpRequest& req, HttpResponseWriter&& resp) {
        std::cout << "Called 2\n";
        std::println("{} {} {}", req.get_query("id2").as_str(), req.get_query("name").as_str(), req.get_query("surname").as_str());
        auto response = HttpResponseBuilder().set_type(ContentType::TEXT).set_body("cock").build();
        resp.respond(response);
    }
  
};