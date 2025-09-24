#include "weblib/server/HttpRequest.hpp"
#include "weblib/server/Utils.hpp"
#include <algorithm>
#include <iterator>
#include <ranges>
#include <iostream>
#include <stdexcept>



void HttpHeaders::extract_headers_from_str(const std::string& request_str) {
    if (request_str.empty()) {
        return; // Empty headers
    }
    std::istringstream strm(request_str); // here
    std::string line;
    while (std::getline(strm, line, '\n')) { // Extracting headers one by one
        utils::trim(line); // Get rid of spaces and carriage returns

        auto pos = line.find(':');
        if (pos == std::string::npos) {
            std::cerr << "Malformed http m_request: no colon\n";
            throw std::runtime_error("Malformed http m_request 1");
        }

        auto name = std::string(line.begin(), line.begin() + static_cast<std::string::difference_type>(pos));
        auto value = std::string(line.begin() + static_cast<std::string::difference_type>(pos) + 2, line.end());
        utils::trim(value);

        auto lowercase_name = utils::to_lowercase_str(name);
        if (lowercase_name != "cookie") {
            m_headers.emplace(lowercase_name, std::move(value)); // Add header
        } else {
            auto values = value 
                | std::views::split(';') 
                | std::views::transform([](auto&& range) {
                    auto val = std::string{range.begin(), range.end()};
                    utils::trim(val);
                    return val;
                }) 
                | std::ranges::to<std::vector<std::string>>();
            std::ranges::for_each(values, [&](auto&& cookie) {
                auto name_value = cookie | std::views::split('=') | std::ranges::to<std::vector<std::string>>();
                if (name_value.size() != 2) {
                    throw std::runtime_error("Malformed http m_request");
                }
                auto const  name_cookie = std::move(name_value.front());
                auto const value_cookie = std::move(name_value.back());
                m_cookies.emplace(utils::to_lowercase_str(name_cookie), Cookie{std::move(name_cookie), std::move(value_cookie)});
            });
        }
    }
}

void HttpQuery::parse_from_string(const std::string& request_str, const std::vector<std::string>& param_names, const std::string& serv_path) {
    if (param_names.empty()) {
        return;
    }
    
    auto param_name_iter = param_names.begin(); // m_param_names stores id, user from api/{id}/{user} (example)
    if (param_name_iter == param_names.end()) {
        throw std::runtime_error("Malformed http m_request");
    }
    
    size_t const endpoint_start = request_str.find('/');
    if (endpoint_start == std::string::npos) {
        throw std::runtime_error("Wtf");
    }
    std::string_view const path{request_str.data() + endpoint_start + 1, request_str.find("HTTP") - endpoint_start - 2}; // additional 1 taking a space into account

    // Slash handling
    std::string_view pattern{serv_path};
    pattern.remove_prefix(1);

    // Processing path arguments
    auto pattern_slash = pattern.substr(0, pattern.find('?'));
    auto pattern_slash_args = pattern_slash 
        | std::views::split('/')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    
    auto path_slash = path.substr(0, path.find('?'));
    auto path_slash_args = path_slash 
        | std::views::split('/')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    if (pattern_slash_args.size() != path_slash_args.size()) {
        throw std::runtime_error("Pattern slash args size != Path slash args size");
    }

    for (const auto&& [pattern_arg, path_arg] : std::views::zip(pattern_slash_args, path_slash_args)) {
        if (pattern_arg.contains('{')) { // Если есть скобка, значит параметр шаблонный
            if (param_name_iter == param_names.end()) { 
                throw std::runtime_error("Malformed http m_request");
            }
            m_parameters.emplace(*param_name_iter, path_arg);
            ++param_name_iter;
        }
    }

    auto pattern_question_pos = pattern.find('?');
    auto path_question_pos = path.find('?');
    auto pattern_query = pattern.substr(pattern_question_pos == std::string::npos ? pattern.size() : pattern_question_pos + 1);
    auto path_query = path.substr(path_question_pos == std::string::npos ? path.size() : path_question_pos + 1);

    auto pattern_query_args = pattern_query 
        | std::views::split('&')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    auto path_query_args = path_query 
        | std::views::split('&')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    if (pattern_query_args.size() != path_query_args.size()) {
        throw std::runtime_error("Pattern query args size != Path query args size");
    }

    auto split_kv_query = [](std::string_view key_value) -> std::pair<std::string_view, std::string_view> {
        auto k_v = key_value 
            | std::views::split('=')
            | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
            | std::ranges::to<std::vector>();
        if (k_v.size() != 2) {
            throw std::runtime_error("Ill-formed m_request");
        }
        return {k_v.front(), k_v.back()};
    };

    for (const auto&& [pattern_kv, path_kv] : std::views::zip(pattern_query_args, path_query_args)) {
        auto [pattern_name, pattern_value] = split_kv_query(pattern_kv);
        auto [path_name, path_value] = split_kv_query(path_kv);
        if (pattern_value.contains('{')) { // Если есть скобка, значит параметр шаблонный
            if (param_name_iter == param_names.end()) {
                throw std::runtime_error("Malformed http m_request");
            }
            m_parameters.emplace(*param_name_iter, path_value);
            ++param_name_iter;
        } else { // Means a static query parameter, not dynamic
            m_parameters.emplace(path_name, path_value); 
        }
    }
}

auto HttpQuery::get_query(const std::string& query_name) const -> Query {
    Query query;
    auto pos = m_parameters.find(utils::to_lowercase_str(query_name));
    if (pos != m_parameters.end()) { //If header exists
        query.m_content = pos->second;
    }
    return query;
}
// TODO: Factor in a separate file



auto HttpHeaders::get_header(const std::string &header_name) const -> std::optional<std::string> {
    auto pos = m_headers.find(utils::to_lowercase_str(header_name));
    if (pos != m_headers.end()) { //If header exists
        return std::optional<std::string>{pos->second};
    }
    return std::nullopt;
}

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