// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include "weblib/server/HttpResponse.hpp"
#include <algorithm>
#include "RequestType.hpp"
#include <span>
#include <vector>
#include "funcs.hpp"

class HttpRequest;

class HttpHandle
{ // HttpHandle data class
  public:
    HttpHandle() = default;

    void add_filter(Filter &&filter);
    void set_handle_method(Handler &&handle);
    void add_http_method(RequestType method);

    [[nodiscard]] auto pass_middlewares(const HttpRequest &request) const -> bool;
    auto               has_method(RequestType type) -> bool
    {
        return std::ranges::any_of(m_methods, [type](auto _type) { return _type == type; });
    }

    void set_parameters(std::vector<std::string> &&vec);
    [[nodiscard]]
    auto get_parameters() const -> std::span<const std::string>
    {
        return m_parameters;
    }

    void               set_path(std::string ep_name);
    [[nodiscard]] auto get_path() const -> std::string { return m_path; }

    void operator()(const HttpRequest &req, HttpResponseWriter &resp) const;

  private:
    std::vector<RequestType> m_methods;
    std::vector<Filter>      m_filters;
    std::vector<std::string> m_parameters;
    std::string              m_path;
    Handler                  m_handle;
};
