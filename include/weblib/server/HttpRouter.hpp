// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include <cstdint>
#include <debug.hpp>
#include <expected>
#include <string_view>

enum class RequestType : std::uint8_t;

class HttpResponseWriter;
class HttpRouter {
public:
    HttpRouter() = default;

    HttpRouter(const HttpRouter&) = delete;
    HttpRouter(HttpRouter &&) = delete;
    HttpRouter& operator=(const HttpRouter&) = delete;
    HttpRouter& operator=(HttpRouter&&) = delete;    

    static auto instance() -> HttpRouter& { // Singleton
        static HttpRouter router{};

        return router;
    }
    void process_request(int client_socket, std::string_view request_string); // Processes string and etc
private:
    static auto parse_request_line(std::string_view request_string) -> std::optional<std::pair<std::string, std::string>>;
    void handle_request(HttpResponseWriter& resp, std::string_view path, std::string_view request_string, std::string_view method, RequestType request_type); // Calls handlers and filters
};