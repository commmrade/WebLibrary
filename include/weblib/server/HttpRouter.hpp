// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include <cstdint>
#include <string_view>
#include <optional>
#include "weblib/debug.hpp"
#include <expected>
#include <string_view>
namespace weblib {
enum class RequestType : std::uint8_t;

class HttpResponseWriter;
class HttpRouter
{
  public:
    HttpRouter() = default;

    HttpRouter(const HttpRouter &)            = delete;
    HttpRouter(HttpRouter &&)                 = delete;
    HttpRouter &operator=(const HttpRouter &) = delete;
    HttpRouter &operator=(HttpRouter &&)      = delete;

    static auto instance() -> HttpRouter &
    { // Singleton
        static HttpRouter router{};

        return router;
    }
    void process_request(int              sock,
                         std::string_view raw_http); // Processes string and etc
  private:
    void handle_request(HttpResponseWriter &resp, std::string_view path,
                        std::string_view raw_http,
                        RequestType request_type); // Calls handlers and filters
};
} // namespace weblib