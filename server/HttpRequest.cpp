#include "weblib/utils.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <algorithm>
#include <iostream>
#include <json/reader.h>
#include <memory>
#include <optional>
#include <print>
#include <ranges>
#include "weblib/debug.hpp"
#include "weblib/consts.hpp"
namespace weblib
{
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

auto HttpRequest::body_as_json() const -> std::optional<Json::Value>
{
    auto has_json =
        m_headers.get_headers().contains(std::string{HeaderConsts::CONTENT_TYPE_APP_JSON});
    if (!has_json)
    {
        return std::nullopt;
    }

    Json::CharReaderBuilder                 builder;
    Json::Value                             obj;
    const std::unique_ptr<Json::CharReader> reader{builder.newCharReader()};
    JSONCPP_STRING                          err;
    if (!reader->parse(m_body.c_str(), m_body.c_str() + m_body.size(), &obj, &err))
    {
        debug::log_error("Could not parse json, because ", err);
        return std::nullopt;
    }
    return {obj};
}

void HttpRequest::extract_queries(const std::string &raw_http, const std::string &path,
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