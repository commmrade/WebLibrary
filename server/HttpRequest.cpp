#include "server/Utils.hpp"
#include <algorithm>
#include <cctype>
#include <iterator>
#include <ostream>
#include <ranges>
#include <server/HttpRequest.hpp>
#include <iostream>
#include <print>
#include <stdexcept>

HttpRequest::HttpRequest(std::string request_str, std::string endpoint_name_str, std::span<const std::string> pnames)
    : request(std::move(request_str)), param_names(std::vector(pnames.begin(), pnames.end())), endpoint_name_str_(std::move(endpoint_name_str)) 
{
    extract_headers();
    extract_queries(); 
}
HttpRequest::HttpRequest(std::string request_str) : request(std::move(request_str)) {
    extract_headers();
}

Query HttpRequest::get_query(const std::string& query_name) const {
    Query query;
    auto pos = parameters.find(utils::to_lowercase_str(query_name));
    if (pos != parameters.end()) { //If header exists
        query.content = pos->second;
    }
    return query;
}

std::optional<std::string> HttpRequest::get_header(const std::string &header_name) const {
    auto pos = headers.find(utils::to_lowercase_str(header_name));
    if (pos != headers.end()) { //If header exists
        return pos->second;
    }
    return std::nullopt;
}

std::optional<Cookie> HttpRequest::get_cookie(const std::string &name) const {
    auto pos = cookies.find(utils::to_lowercase_str(name));
    if (pos != cookies.end()) {
        return pos->second;
    }
    return std::nullopt;
}

std::unique_ptr<Json::Value> HttpRequest::body_as_json() const {
    const std::string raw_json = request.substr(request.find("\r\n\r\n") + 4);
    Json::Value json_obj{};


    Json::Reader json_reader;
    if (!json_reader.parse(raw_json, json_obj)) {
        return nullptr;
    }
    return std::make_unique<Json::Value>(std::move(json_obj)); // Hopefuly Json::Value is good at move semantics
}

void HttpRequest::extract_queries() {
    if (param_names.empty()) {
        return;
    }
    
    auto param_name_iter = param_names.begin(); // param_names stores id, user from api/{id}/{user} (example)
    if (param_name_iter == param_names.end()) {
        throw std::runtime_error("Malformed http request");
    }
    
    size_t const endpoint_start = request.find("/");
    if (endpoint_start == std::string::npos) {
        throw std::runtime_error("Wtf");
    }
    std::string_view const path{request.data() + endpoint_start + 1, request.find("HTTP") - endpoint_start - 2}; // additional 1 taking a space into account

    // Slash handling
    std::string_view pattern{endpoint_name_str_};
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
            if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
            parameters.emplace(*param_name_iter, path_arg);
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
            throw std::runtime_error("Ill-formed request");
        }
        return {k_v.front(), k_v.back()};
    };

    for (const auto&& [pattern_kv, path_kv] : std::views::zip(pattern_query_args, path_query_args)) {
        auto [pattern_name, pattern_value] = split_kv_query(pattern_kv);
        auto [path_name, path_value] = split_kv_query(path_kv);
        if (pattern_value.contains('{')) { // Если есть скобка, значит параметр шаблонный
            if (param_name_iter == param_names.end()) {
                throw std::runtime_error("Malformed http request");
            }
            parameters.emplace(*param_name_iter, path_value);
            ++param_name_iter;
        } else { // Means a static query parameter, not dynamic
            parameters.emplace(path_name, path_value); 
        }
    }
}


void HttpRequest::extract_headers() {
    auto header_section_start = request.find("\r\n") + 2;
    auto header_section_end = request.find("\r\n\r\n");
    if (header_section_start == header_section_end) {
        return; // Empty headers
    }
    std::istringstream strm(request.substr(header_section_start, header_section_end - header_section_start)); // here
    std::string line;
    while (std::getline(strm, line, '\n')) { // Extracting headers one by one
        utils::trim(line); // Get rid of spaces and carriage returns

        auto pos = line.find(':');
        if (pos == std::string::npos) {
            std::cerr << "Malformed http request: no colon\n";
            throw std::runtime_error("Malformed http request 1");
        }

        auto name = std::string(line.begin(), line.begin() + pos);
        auto value = std::string(line.begin() + pos + 1, line.end());
        utils::trim(value);

        auto lowercase_name = utils::to_lowercase_str(name);
        if (lowercase_name != "cookie") {
            headers.emplace(lowercase_name, std::move(value)); // Add header
        } else { // If it is a cookie
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
                    throw std::runtime_error("Malformed http request");
                }
                auto const  name = std::move(name_value.front());
                auto const value = std::move(name_value.back());
                cookies.emplace(lowercase_name, Cookie{std::move(name), std::move(value)});
            });
        }

    }
    
}