// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include "weblib/server/Cookie.hpp"
#include <iostream>
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
#include "weblib/server/Utils.hpp"
#include "weblib/server/HttpQuery.hpp"
#include "weblib/server/HttpHeaders.hpp"

class HttpRequest {
public:
    explicit HttpRequest(std::string request_str, std::string endpoint_name_str, std::span<const std::string> pnames = {});

    [[nodiscard]]
    auto get_query(const std::string& query_name) const -> Query;

    [[nodiscard]]
    auto get_queries() const -> QueryView {
        return m_query.get_queries();
    }

    [[nodiscard]]
    auto get_header(const std::string &header_name) const -> std::optional<std::string>;
    [[nodiscard]]
    auto get_headers() const -> HeaderView {
        return m_headers.get_headers();
    }

    [[nodiscard]]
    auto get_cookie(const std::string &name) const -> std::optional<Cookie>;

    [[nodiscard]]
    auto get_cookies() const -> CookieView {
        return m_headers.get_cookies();
    }

    [[nodiscard]]
    auto body_as_str() const -> std::string { return m_body; }

    [[nodiscard]]
    auto body_as_json() const -> std::unique_ptr<Json::Value>;


    [[nodiscard]]
    auto get_method() const -> RequestType {
        return req_type_from_str(m_method);
    }

    [[nodiscard]]
    auto get_version() const -> std::string {
        return m_version;
    }

    void set_header(const std::string &name, std::string_view value) {
        m_headers.set_header(name, value);
    }
private:
    // std::string m_request;

    HttpHeaders m_headers;
    HttpQuery m_query;
    
    std::string m_method;
    std::string m_body;

    std::string m_version;

    void extract_queries(const std::string& request_str, const std::string& endpoint_name_str, std::span<const std::string> pnames);
    void extract_headers(const std::string& request_str);
};
