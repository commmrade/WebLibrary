#pragma once
#include "json/writer.h"
#include <json/value.h>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <sys/socket.h>
#include<unordered_map>
#include<string>
#include "Cookie.hpp"
#include <json/json.h>
#include <json/json.h>
#include <debug.hpp>
#include <fstream>
#include "server/HeaderView.hpp"
#include "types.hpp"

class HttpResponseBuilder;
enum class HeaderType;
enum class ResponseType; 

class HttpResponse {
private:
    
    std::unordered_map<std::string, std::string> headers{{"Content-Type", "plain/text"}};
    std::string body{};
    int status_code{200};

    std::string status_message{"OK"};

    std::string http_version{"1.1"};

public:
    HttpResponse() = default;

    [[nodiscard]]
    std::string to_string() const;

    void set_header_raw(const std::string &name, std::string_view value);
    void set_header(HeaderType header_type, std::string_view value);
    void remove_header(const std::string &name);
    void set_content_type(ContentType type);
    void set_cookie(const Cookie &cookie);

    void set_body(std::string text) {
        body = std::move(text);
    }
    void set_body_json(const Json::Value& json_obj);

    void set_status(int status_code);

    void set_version(std::string_view http_ver) {
        http_version = http_ver;
    }

    void set_custom_message(std::string msg) {
        status_message = std::move(msg);
    }
};

class HttpResponseWriter {
public:    
    HttpResponseWriter(int client_socket) : client_socket(client_socket) {}
    HttpResponseWriter(const HttpResponseWriter&) = delete;
    HttpResponseWriter& operator=(const HttpResponseWriter&) = delete;

    void respond(HttpResponse &resp);
private:
    int client_socket;
};

class HttpResponseBuilder {
private:
    HttpResponse resp{};
public:
    HttpResponseBuilder() {}
    
    [[nodiscard]] HttpResponseBuilder& set_body_str(std::string text) {
        resp.set_body(std::move(text));
        return *this;
    }
    [[nodiscard]] HttpResponseBuilder& set_body_json(const Json::Value& json_obj) {
        resp.set_body_json(json_obj);
        return *this;
    }

    [[nodiscard]] HttpResponseBuilder& set_status(int code) {
        resp.set_status(code);
        return *this;
    }
    [[nodiscard]] HttpResponseBuilder& set_content_type(ContentType type) {
        resp.set_content_type(type);
        return *this;
    }
    [[nodiscard]] HttpResponseBuilder& set_custom_message(const std::string &msg) {
        resp.set_custom_message(msg);
        return *this;
    }
    
    [[nodiscard]] HttpResponse build() {
        return std::move(resp);
    }
};



