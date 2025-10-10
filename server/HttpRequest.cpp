#include "weblib/utils.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <algorithm>
#include <iostream>
#include <print>
#include <ranges>
#include "weblib/consts.hpp"
namespace weblib {
HttpRequest::HttpRequest(std::string raw_http, std::string path,
                         std::span<const std::string> pnames)
{
    extract_headers(raw_http);
    extract_queries(raw_http, path, pnames);

    m_method = raw_http.substr(0, raw_http.find(' '));
    m_body   = raw_http.substr(raw_http.find(HttpConsts::CRNLCRNL) + HttpConsts::CRNLCRNL.size());

    auto line = raw_http.substr(0, raw_http.find(HttpConsts::CRNL));
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
    auto header_start_pos = raw_http.find(HttpConsts::CRNL) + HttpConsts::CRNL.size();
    auto header_end_pos   = raw_http.find(HttpConsts::CRNLCRNL);
    auto headers_section  = raw_http.substr(header_start_pos, header_end_pos - header_start_pos);
    m_headers.parse_from_string(headers_section);
}
} // namespace weblib