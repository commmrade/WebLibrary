#pragma once
#include "json/writer.h"
#include <json/value.h>
#include <optional>
#include <stdexcept>
#include <sys/socket.h>
#include<unordered_map>
#include<string>
#include "Cookie.hpp"
#include <json/json.h>
#include <json/json.h>
#include <debug.hpp>

enum class HeaderType {
    CONTENT_TYPE,
    CONTENT_LENGTH,
    AUTH_BEARER,
    AUTH_BASIC,
};

enum class ResponseType {
    HTML,
    JSON,
    TEXT,
};



class Response {
private:
    
    std::unordered_map<std::string, std::string> headers;
    std::string body{};
    int status_code{200};

    std::string status_message{"OK"};

    std::string http_version{"1.1"};

public:

    Response(ResponseType type);
    Response(int status_code, const std::string &resp_text, ResponseType type = ResponseType::TEXT);


    [[nodiscard]]
    std::string respond_text() const;

    void add_header_raw(std::string name, std::string value);

    void add_cookie(const Cookie &cookie);

    void add_header(HeaderType header_type, std::string value);

    void remove_header(const std::string &name);

    void set_type(ResponseType type);

    [[nodiscard]]
    std::optional<std::string> get_header(const std::string &name) const;

    void set_body(const std::string &text) {
        body = text;
    }
    void set_body(const char *text) {
        body = text;
    }
    void set_body(const Json::Value& json_obj);
    
    [[nodiscard]] 
    std::string get_body() const {
        return body;
    }

    void set_status(int status_code);
    [[nodiscard]]
    int get_status() const {
        return status_code;
    }

    void set_version(const std::string &http_ver) {
        http_version = http_ver;
    }
    [[nodiscard]]
    std::string get_version() const {
        return http_version;
    }

    void set_custom_message(const std::string &msg) {
        status_message = msg;
    }
    [[nodiscard]]
    std::string get_status_message() const { 
        return status_message;
    }

};

class HttpResponse {
public:    
    HttpResponse(int client_socket) : client_socket(client_socket) {
        
    }
    HttpResponse(const HttpResponse&) = delete;
    HttpResponse& operator=(const HttpResponse&) = delete;

    void respond(Response &resp);
private:
    int client_socket;
};


