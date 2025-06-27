// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include "json/writer.h"
#include <json/value.h>
#include <sys/socket.h>
#include<unordered_map>
#include<string>
#include "Cookie.hpp"
#include <json/json.h>
#include <json/json.h>
#include <debug.hpp>
#include "weblib/server/HeaderView.hpp"
#include "types.hpp"

class HttpResponseBuilder;
enum class HeaderType : std::uint8_t;
enum class ResponseType : std::uint8_t; 

inline constexpr int MAX_WAIT = 5000;

enum StatusCode {
    OK = 200,
    CREATED = 201,
    NO_CONTENT = 204,
    BAD_REQUEST = 400,
    UNAUTHORIZED = 401,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    INTERNAL_SERVER_ERROR = 500,
    BAD_GATEWAY = 502,
    SERVICE_UNAVAILABLE = 503,
    GATEWAY_TIMEOUT = 504
};


class HttpResponse {
private:
    
    std::unordered_map<std::string, std::string> m_headers{{"Content-Type", "plain/text"}};
    std::string m_body{};
    int m_status_code{200};

    std::string m_status_message{"OK"};

    std::string m_http_version{"1.1"};

public:
    HttpResponse() = default;

    [[nodiscard]]
    auto to_string() const -> std::string;

    void set_header_raw(const std::string &name, std::string_view value);
    void set_header(HeaderType header_type, std::string_view value);
    void remove_header(const std::string &name);
    void set_content_type(ContentType type);
    void set_cookie(const Cookie &cookie);

    void set_body(std::string text) {
        m_body = std::move(text);
    }
    void set_body_json(const Json::Value& json_obj);

    void set_status(int status_code);

    void set_version(std::string_view http_ver) {
        m_http_version = http_ver;
    }

    void set_custom_message(std::string msg) {
        m_status_message = std::move(msg);
    }
};

class HttpResponseWriter {
public:    
    HttpResponseWriter(int client_socket) : m_client_socket(client_socket) {}
    HttpResponseWriter(const HttpResponseWriter&) = delete;
    HttpResponseWriter& operator=(const HttpResponseWriter&) = delete;

    void respond(HttpResponse &resp);
private:
    int m_client_socket;
};

class HttpResponseBuilder {
private:
    HttpResponse resp{};
public:
    HttpResponseBuilder() = default;
    
    [[nodiscard]] auto set_body_str(std::string text) -> HttpResponseBuilder& {
        resp.set_body(std::move(text));
        return *this;
    }
    [[nodiscard]] auto set_body_json(const Json::Value& json_obj) -> HttpResponseBuilder& {
        resp.set_body_json(json_obj);
        return *this;
    }

    [[nodiscard]] auto set_status(int code) -> HttpResponseBuilder& {
        resp.set_status(code);
        return *this;
    }
    [[nodiscard]] auto set_content_type(ContentType type) -> HttpResponseBuilder& {
        resp.set_content_type(type);
        return *this;
    }
    [[nodiscard]] auto set_custom_message(const std::string &msg) -> HttpResponseBuilder& {
        resp.set_custom_message(msg);
        return *this;
    }
    
    [[nodiscard]] auto build() -> HttpResponse {
        return std::move(resp);
    }
};



