#pragma once

#include "server/Cookie.hpp"
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
#include "server/RequestType.hpp"

class HttpRequest {
public:
    explicit HttpRequest(std::string request_str, std::string endpoint_name_str, std::span<const std::string> pnames = {});
    explicit HttpRequest(std::string request_str); // For parsing only headers

    [[nodiscard]]
    Query get_query(const std::string& query_name) const;

    [[nodiscard]]
    QueryView get_queries() const {
        return QueryView{m_parameters};
    }

    [[nodiscard]]
    std::optional<std::string> get_header(const std::string &header_name) const;
    [[nodiscard]]
    HeaderView get_headers() const {
        return HeaderView{m_headers};
    }

    [[nodiscard]]
    std::optional<Cookie> get_cookie(const std::string &name) const;

    [[nodiscard]] 
    CookieView get_cookies() const {
        return CookieView{m_cookies};
    }

    [[nodiscard]]
    std::string body_as_str() const { return m_request.substr(m_request.find("\r\n\r\n") + 4); }

    [[nodiscard]]
    std::unique_ptr<Json::Value> body_as_json() const;


    [[nodiscard]]
    RequestType get_method() const { 
        auto method_str = m_request.substr(0, m_request.find(" ")); 
        return req_type_from_str(method_str);
    }

    [[nodiscard]]
    std::string get_version() const {
        auto line = m_request.substr(0, m_request.find("\r\n"));
        return line.substr(line.find_last_of("/") + 1);
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