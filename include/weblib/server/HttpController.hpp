// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include<cstdarg>
#include <utility>
#include "debug.hpp"
#include "weblib/server/HttpBinder.hpp"

#define mv(X) std::move(X)         
#define REG_ENDPOINT(FUNCTION, NAME, TYPE, ...) register_method(NAME, [this] (const HttpRequest &req, HttpResponseWriter &resp) { FUNCTION(req, mv(resp)); }, TYPE, __VA_ARGS__)


template<typename Derived> // Currently no real need for CRTP  but maybe in the future
class HttpController {
public:
    HttpController() = default;
    HttpController(const HttpController&) = delete;
    HttpController(HttpController &&) = delete;
    HttpController& operator=(const HttpController&) = delete;
    HttpController& operator=(HttpController&&) = delete;


    template<typename ... RequestTypes>
    static void register_method(const std::string &endpoint_name, Handler &&handler, RequestTypes&&... types) {
        debug::log_info("Registering a handler");
        HttpBinder::instance().register_handler(endpoint_name, std::move(handler), std::forward<RequestTypes>(types)...);
    }
};


