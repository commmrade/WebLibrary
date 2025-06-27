// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include "weblib/server/Cookie.hpp"
#include <json/reader.h>
#include <json/value.h>
#include <memory>
#include <optional>
#include <span>
#include<unordered_map>
#include<string>
#include <json/json.h>
#include <vector>
#include "Query.hpp"
#include "HeaderView.hpp"
#include "weblib/server/RequestType.hpp"

class HttpRequest {
public:
    explicit HttpRequest(std::string request_str, std::string endpoint_name_str, std::span<const std::string> pnames = {});
    explicit HttpRequest(std::string request_str); // For parsing only headers

    [[nodiscard]]
    auto get_query(const std::string& query_name) const -> Query;

    [[nodiscard]]
    auto get_queries() const -> QueryView {
        return QueryView{m_parameters};
    }

    [[nodiscard]]
    auto get_header(const std::string &header_name) const -> std::optional<std::string>;
    [[nodiscard]]
    auto get_headers() const -> HeaderView {
        return HeaderView{m_headers};
    }

    [[nodiscard]]
    auto get_cookie(const std::string &name) const -> std::optional<Cookie>;

    [[nodiscard]] 
    auto get_cookies() const -> CookieView {
        return CookieView{m_cookies};
    }

    [[nodiscard]]
    auto body_as_str() const -> std::string { return m_request.substr(m_request.find("\r\n\r\n") + 4); }

    [[nodiscard]]
    auto body_as_json() const -> std::unique_ptr<Json::Value>;


    [[nodiscard]]
    auto get_method() const -> RequestType { 
        auto method_str = m_request.substr(0, m_request.find(' ')); 
        return req_type_from_str(method_str);
    }

    [[nodiscard]]
    auto get_version() const -> std::string {
        auto line = m_request.substr(0, m_request.find("\r\n"));
        return line.substr(line.find_last_of('/') + 1);
    }

    void add_header(const std::string &name, std::string_view value) const {
        m_headers[name] = value;
    }
private:
    std::string m_request;
    std::unordered_map<std::string, std::string> m_parameters;
    mutable std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<int, std::string> m_path_params;
    std::unordered_map<std::string, Cookie> m_cookies;

    std::vector<std::string> m_param_names;
    std::string m_endpoint_name_str;

    void extract_queries();
    void extract_headers();  
};