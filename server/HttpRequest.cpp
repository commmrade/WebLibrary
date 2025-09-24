#include "weblib/server/Utils.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <algorithm>
#include <iostream>
#include <ranges>


HttpRequest::HttpRequest(std::string request_str, std::string endpoint_name_str, std::span<const std::string> pnames)
    : m_request(std::move(request_str)), m_parameters(std::vector(pnames.begin(), pnames.end())), m_path(endpoint_name_str)
{
    extract_headers();
    extract_queries(); 
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
    const std::string raw_json = m_request.substr(m_request.find("\r\n\r\n") + 4);
    Json::Value json_obj{};


    Json::Reader json_reader;
    if (!json_reader.parse(raw_json, json_obj)) {
        return nullptr;
    }
    return std::make_unique<Json::Value>(std::move(json_obj)); // Hopefuly Json::Value is good at move semantics
}

void HttpRequest::extract_queries() {
   m_query.parse_from_string(m_request, m_parameters, m_path);
}


void HttpRequest::extract_headers() {
    auto header_start_pos = m_request.find("\r\n") + 2;
    auto header_end_pos = m_request.find("\r\n\r\n");
    auto headers_section = m_request.substr(header_start_pos, header_end_pos - header_start_pos);
    m_headers.parse_from_string(headers_section);
}