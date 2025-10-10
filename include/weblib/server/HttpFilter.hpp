// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include "weblib/server/HttpBinder.hpp"
#include "weblib/server/HttpRequest.hpp"
#include "weblib/server/HttpServer.hpp"
#include "weblib/server/HttpRouter.hpp"

#define mv(X) std::move(X)
#define REG_FILTER(NAME, FUNCTION)                                                                 \
    HttpFilter::register_filter(NAME, [this](const weblib::HttpRequest &req)                               \
                                { return FUNCTION((req)); }) // Macro to register filter

namespace weblib {
template <typename Derived> // CRTP needed to avoid dynamic dispatching
class HttpFilter
{
  public:
    HttpFilter()                              = default;
    HttpFilter(const HttpFilter &)            = delete;
    HttpFilter(HttpFilter &&)                 = delete;
    HttpFilter &operator=(const HttpFilter &) = delete;
    HttpFilter &operator=(HttpFilter &&)      = delete;

    [[nodiscard]] auto do_filter(const HttpRequest &req) -> bool
    {
        return static_cast<Derived *>(this)->doFilter(req);
    }

    static void register_filter(std::string_view route, Filter &&filter)
    {
        debug::log_info("Registering a filter");
        HttpBinder::instance().register_filter(route, std::move(filter));
    }
};
} // namespace weblib