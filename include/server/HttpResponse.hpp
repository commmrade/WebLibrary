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

    void add_header_raw(const std::string &name, std::string_view value);
    void add_header(HeaderType header_type, std::string_view value);
    const std::unordered_map<std::string, std::string>& get_headers() const { return headers; }

    void add_cookie(const Cookie &cookie);
    void remove_header(const std::string &name);

    void set_type(ContentType type);

    [[nodiscard]]
    std::optional<std::string> get_header(const std::string &name) const;

    void set_body(const std::string & text) {
        body = text;
    }
    void set_body(const char *text) {
        body = text;
    }
    void set_body(const Json::Value& json_obj);
    [[nodiscard]] 
    std::string get_body_str() const {
        return body;
    }

    void set_status(int status_code);
    [[nodiscard]]
    int get_status() const {
        return status_code;
    }

    void set_version(std::string_view http_ver) {
        http_version = http_ver;
    }
    [[nodiscard]]
    std::string get_version() const {
        return http_version;
    }

    void set_custom_message(std::string_view msg) {
        status_message = msg;
    }
    [[nodiscard]]
    std::string get_status_message() const { 
        return status_message;
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
    
    [[nodiscard]] HttpResponseBuilder& set_body(const std::string& text) {
        resp.set_body(text);
        return *this;
    }
    [[nodiscard]] HttpResponseBuilder& set_body(const char *text) {
        resp.set_body(text);
        return *this;
    }
    [[nodiscard]] HttpResponseBuilder& set_body(const Json::Value& json_obj) {
        resp.set_body(json_obj);
        return *this;
    }

    [[nodiscard]] HttpResponseBuilder& set_status(int code) {
        resp.set_status(code);
        return *this;
    }
    [[nodiscard]] HttpResponseBuilder& set_type(ContentType type) {
        resp.set_type(type);
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



