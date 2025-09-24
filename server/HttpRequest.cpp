#include "weblib/server/Utils.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>


HttpRequest::HttpRequest(std::string request_str, std::string path, std::span<const std::string> pnames)
{
    extract_headers(request_str);
    extract_queries(request_str, path, pnames); 

    m_method = request_str.substr(0, request_str.find(' '));
    m_body = request_str.substr(request_str.find("\r\n\r\n") + 4);

    auto line = request_str.substr(0, request_str.find("\r\n"));
    m_version = line.substr(line.find_last_of('/') + 1);
}

auto HttpRequest::get_query(const std::string& query_name) const -> Query {
    return m_query.get_query(query_name);
}

auto HttpRequest::get_header(const std::string &header_name) const -> std::optional<std::string> {
    return m_headers.get_header(header_name);
}

auto HttpRequest::get_cookie(const std::string &name) const -> std::optional<Cookie> {
    return m_headers.get_cookie(name);
}

auto HttpRequest::body_as_json() const -> std::unique_ptr<Json::Value> {
    Json::Value json_obj{};
    Json::Reader json_reader;
    if (!json_reader.parse(m_body, json_obj)) {
        return nullptr;
    }
    return std::make_unique<Json::Value>(std::move(json_obj));
}

void HttpRequest::extract_queries(const std::string& request_str, const std::string& endpoint_name_str, std::span<const std::string> pnames) {
    auto params = std::vector<std::string>{pnames.begin(), pnames.end()};
    m_query.parse_from_string(request_str, params, endpoint_name_str);
}


void HttpRequest::extract_headers(const std::string& request_str) {
    auto header_start_pos = request_str.find("\r\n") + 2;
    auto header_end_pos = request_str.find("\r\n\r\n");
    auto headers_section = request_str.substr(header_start_pos, header_end_pos - header_start_pos);
    m_headers.parse_from_string(headers_section);
}