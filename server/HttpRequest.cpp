#include "weblib/server/Utils.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>

static inline constexpr const char* const CRCRNLNL = "\r\n\r\n"; 
static inline constexpr const char* const CRNL = "\r\n";

HttpRequest::HttpRequest(std::string raw_http, std::string path,
                         std::span<const std::string> pnames)
{
    extract_headers(raw_http);
    extract_queries(raw_http, path, pnames);

    m_method = raw_http.substr(0, raw_http.find(' '));
    m_body   = raw_http.substr(raw_http.find(CRCRNLNL) + 4);

    auto line = raw_http.substr(0, raw_http.find(CRNL));
    m_version = line.substr(line.find_last_of('/') + 1);
}

auto HttpRequest::get_query(const std::string &query_name) const -> Query
{
    return m_query.get_query(query_name);
}

auto HttpRequest::get_header(const std::string &header_name) const -> std::optional<std::string>
{
    return m_headers.get_header(header_name);
}

auto HttpRequest::get_cookie(const std::string &name) const -> std::optional<Cookie>
{
    return m_headers.get_cookie(name);
}

auto HttpRequest::body_as_json() const -> std::unique_ptr<Json::Value>
{
    Json::Value  json_obj{};
    Json::Reader json_reader;
    if (!json_reader.parse(m_body, json_obj))
    {
        return nullptr;
    }
    return std::make_unique<Json::Value>(std::move(json_obj));
}

void HttpRequest::extract_queries(const std::string           &raw_http,
                                  const std::string           &path,
                                  std::span<const std::string> pnames)
{
    auto params = std::vector<std::string>{pnames.begin(), pnames.end()};
    m_query.parse_from_string(raw_http, params, path);
}

void HttpRequest::extract_headers(const std::string &raw_http)
{
    auto header_start_pos = raw_http.find(CRNL) + 2;
    auto header_end_pos   = raw_http.find(CRCRNLNL);
    auto headers_section  = raw_http.substr(header_start_pos, header_end_pos - header_start_pos);
    m_headers.parse_from_string(headers_section);
}