#pragma once
#include "server/HttpController.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <format>
#include <json/value.h>
#include <server/HttpRequest.hpp>
#include <thread>
#include <iostream>

class MyController : public HttpController<MyController>
{
  public:
    MyController()
    {
        // REG_ENDPOINT(reg, "/reg", RequestType::POST, RequestType::OPTIONS);

        REG_ENDPOINT(reg, "/courses", RequestType::GET, RequestType::OPTIONS);
        REG_ENDPOINT(test_r, "/courses/{course_id}?name={name}&age={age}", RequestType::GET,
                     RequestType::OPTIONS);
        REG_ENDPOINT(test_r2, "/courses/{course_id}?name={name}&skill=big", RequestType::GET,
                     RequestType::OPTIONS);
        REG_ENDPOINT(smth2, "/courses/{course_id}/modules/{module_id}", RequestType::GET,
                     RequestType::OPTIONS);
    }

  protected:
    void reg(const HttpRequest &req, HttpResponseWriter &&resp)
    {

        auto response = HttpResponseBuilder{}.set_body_str("Hello friend").build();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        resp.respond(response);
    }

    void test_r(const HttpRequest &req, HttpResponseWriter &&resp)
    {
        auto name = req.get_query("name").as_str();
        auto age  = req.get_query("age").as<int>();
        std::println("{} {}", name, age);

        auto view = req.get_headers();
        for (const auto &[key, value] : view)
        {
            std::cout << "Header: " << key << " = " << value << "\n";
        }

        auto cookies_view = req.get_cookies();
        for (const auto &[key, value] : cookies_view)
        {
            std::println("{} {}", key, value.get_value());
        }

        auto response =
            HttpResponseBuilder().set_content_type(ContentType::TEXT).set_body_str("test").build();
        resp.respond(response);
    }

    void test_r2(const HttpRequest &req, HttpResponseWriter &&resp)
    {
        auto name = req.get_query("name").as_str();
        auto age  = req.get_query("skill").as_str();
        std::println("{} {}", name, age);
        auto response =
            HttpResponseBuilder().set_content_type(ContentType::TEXT).set_body_str("test").build();
        resp.respond(response);
    }

    void smth([[maybe_unused]] const HttpRequest &req, HttpResponseWriter &&resp)
    {
        std::cout << "Called\n";
        auto response = HttpResponseBuilder()
                            .set_content_type(ContentType::TEXT)
                            .set_body_str("dasasds")
                            .build();
        resp.respond(response);
    }
    void smth2(const HttpRequest &req, HttpResponseWriter &&resp)
    {
        std::cout << "Called 2\n";
        auto response = HttpResponseBuilder()
                            .set_content_type(ContentType::TEXT)
                            .set_body_str("sdadasdasdas")
                            .build();
        resp.respond(response);
    }
};