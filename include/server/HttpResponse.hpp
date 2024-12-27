#pragma once
#include "json/writer.h"
#include <json/value.h>
#include <optional>
#include <stdexcept>
#include <sys/socket.h>
#include<unordered_map>
#include<string>
#include<iostream>
#include "Cookie.hpp"
#include <json/json.h>
#include <json/json.h>

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

    Response(ResponseType type) {
        switch (type) {
            case ResponseType::HTML: {
                headers["Content-Type"] = "text/html";
                break;
            }   
            case ResponseType::JSON: {
                headers["Content-Type"] = "application/json";
            }
            case ResponseType::TEXT: {
                // Deliberately made without break
            }
            default: {
                headers["Content-Type"] = "plain/text";
                break;
            }
        }
        headers["Content-Type"] = "text/plain";
    }

    Response(int status_code, const std::string &resp_text, ResponseType type = ResponseType::TEXT) : Response(type) {
        set_body(resp_text);
        set_status(status_code);
    }

    [[nodiscard]]
    std::string respond_text() const {
        std::string response{};
        std::string status_code_str = std::to_string(status_code);
        
        response += "HTTP/" + http_version + " " + status_code_str + " " + status_message + "\r\n";
    
        // Adding headers part
        for (const auto &[header_name, header_value] : headers) {
            response += header_name + ": " + header_value + "\r\n";
        }
        if (headers.find("Content-Length") == headers.end()) { // Setting Content-Length if not already set
            response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
        }

        response += "\r\n"; // Separating headers and answer part

        response += body; // Adding user text

        return response;
    }


    void add_header_raw(std::string name, std::string value) {
        if (name == "Set-Cookie") {
            throw std::runtime_error("Use add_cookie() instead!");
        }

        headers[name] = value;
    }

    void add_cookie(const Cookie &cookie) {
        std::string cookie_str = cookie.get_string();
        // Maybe add multiple
        headers["Set-Cookie"] = cookie_str;
    }

    void add_header(HeaderType header_type, std::string value) {
        switch (header_type) {
            case HeaderType::CONTENT_TYPE: {
                headers["Content-Type"] = value;
                break;
            }
            case HeaderType::CONTENT_LENGTH: {
                headers["Content-Length"] = value;
                break;
            }
            case HeaderType::AUTH_BASIC: {
                headers["Authorization"] = "Basic " + value;
                break;
            }
            case HeaderType::AUTH_BEARER: {
                headers["Authorization"] = "Bearer " + value;
                break;
            }
            default: {
                throw std::runtime_error("Header type is not implemented > todo!");
                break;
            }
        }
    }

    void remove_header(const std::string &name) {
        headers.erase(name); // Removing if exists (doesn't throw if does not exist)
    }

    [[nodiscard]]
    inline std::optional<std::string> get_header(const std::string &name) const {
        if (auto hdr = headers.find(name); hdr != headers.end()) {
            return hdr->second;
        }
        return std::nullopt;
    }

    void set_body(const std::string &text) {
        body = text;
    }

    void set_body(const Json::Value& json_obj) {
        Json::FastWriter json_writer;
        const std::string json_str = json_writer.write(json_obj);
      

        body = json_str;
    }
    void set_body(const char *text) {
        body = text;
    }

    [[nodiscard]] 
    inline std::string get_body() const {
        return body;
    }

    /* inline void set_body(const nlohmann::json &text) {
        body = nlohmann::json::to_string(text); 
    } kinda shit ok
    */

    void set_status(int status_code) {
     
        switch (status_code) {
            case 200: {
                status_message = "OK";
                break;
            }
            case 400: {
                status_message = "Bad request";
                break;
            }
            case 401: {
                status_message = "Unauthorized";
                break;
            }
            case 404: {
                status_message = "Not found";
                break;
            }
            case 500: {
                status_message = "Internal server error";
                break;
            }
            default: {
                status_message = "OK";
                break;
            }

        }
        this->status_code = status_code;
    }
    [[nodiscard]]
    inline int get_status() const {
        return status_code;
    }

    void set_version(const std::string &http_ver) {
        http_version = http_ver;
    }
    [[nodiscard]]
    inline std::string get_version() const {
        return http_version;
    }

    void set_custom_message(const std::string &msg) {
        status_message = msg;
    }
    [[nodiscard]]
    inline std::string get_status_message() const { 
        return status_message;
    }

};

class HttpResponse {
public:    
    HttpResponse(int client_socket) : client_socket(client_socket) {
        
    }
    HttpResponse(const HttpResponse&) = delete;
    HttpResponse& operator=(const HttpResponse&) = delete;

    void respond(Response &resp) { // Sending response text to the requester
        auto response = resp.respond_text();

        ssize_t bytes_sent = send(client_socket, response.c_str(), response.size(), 0);
        if (bytes_sent < 0) {
            perror("Send error");
        } 
    }
private:
    int client_socket;

};


