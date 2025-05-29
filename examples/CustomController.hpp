#pragma once


#include "server/Cookie.hpp"
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <algorithm>
#include <format>
#include <json/value.h>
#include <ostream>
#include <server/HttpRequest.hpp>
#include <vector>





class MyController : public HttpController<MyController> {
public:
    MyController() {
        // REG_ENDPOINT(reg, "/reg", RequestType::POST, RequestType::OPTIONS);
        REG_ENDPOINT(smth, "/courses/{course_id}?name={name}", RequestType::GET, RequestType::OPTIONS);
        REG_ENDPOINT(test_r, "/courses/{course_id}?name={name}&age={age}", RequestType::GET, RequestType::OPTIONS);
        REG_ENDPOINT(test_r2, "/courses/{course_id}?name={name}&skill=big", RequestType::GET, RequestType::OPTIONS);
        REG_ENDPOINT(smth2, "/courses/{course_id}/modules/{module_id}", RequestType::GET, RequestType::OPTIONS);
    }
protected:
    void reg(const HttpRequest& req, HttpResponseWriter&& resp) {
        
        auto body = req.body_as_json();
        auto response = HttpResponseBuilder().set_type(ContentType::JSON).set_body(*body).build();

        resp.respond(response);
    }

    void test_r(const HttpRequest& req, HttpResponseWriter&& resp) {
        auto name = req.get_query("name").as_str();
        auto age = req.get_query("age").as<int>();
        std::println("{} {}", name, age);
       
        auto view = req.get_headers();
        // auto se = std::move(headers);
        // Using std::ranges::for_each

        // Range-based for loop
        for (const auto& [key, value] : view) {
            std::cout << "Header: " << key << " = " << value << "\n";
        }


        auto response = HttpResponseBuilder().set_type(ContentType::TEXT).set_body("test").build();
        resp.respond(response);
    }


    void test_r2(const HttpRequest& req, HttpResponseWriter&& resp) {
        auto name = req.get_query("name").as_str();
        auto age = req.get_query("skill").as_str();
        std::println("{} {}", name, age);
        auto response = HttpResponseBuilder().set_type(ContentType::TEXT).set_body("test").build();
        resp.respond(response);
    }


    void smth([[maybe_unused]] const HttpRequest& req, HttpResponseWriter&& resp) {
        std::cout << "Called\n";
        auto response = HttpResponseBuilder().set_type(ContentType::TEXT).set_body("dasasds").build();
        resp.respond(response);
    }
    void smth2(const HttpRequest& req, HttpResponseWriter&& resp) {
        std::cout << "Called 2\n";
        auto response = HttpResponseBuilder().set_type(ContentType::TEXT).set_body("sdadasdasdas").build();
        resp.respond(response);
    }
  
};