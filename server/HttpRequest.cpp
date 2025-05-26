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

HttpRequest::HttpRequest(const std::string &request_str, std::string endpoint_name_str, std::span<const std::string> param_names)
    : request(request_str), param_names(std::vector(param_names.begin(), param_names.end())), endpoint_name_str_(std::move(endpoint_name_str)) 
{
    extract_headers();
    extract_queries();
}

[[nodiscard]]
Query HttpRequest::get_query(const std::string& query_name) const {
    Query query;
    auto pos = parameters.find(utils::to_lowercase_str(query_name));
    if (pos != parameters.end()) { //If header exists
        query.content = pos->second;
    }
    return query;
}

[[nodiscard]]
std::optional<std::string> HttpRequest::get_header(const std::string &header_name) const {
    auto pos = headers.find(utils::to_lowercase_str(header_name));
    if (pos != headers.end()) { //If header exists
        return pos->second;
    }
    return std::nullopt;
}


[[nodiscard]]
std::optional<Cookie> HttpRequest::get_cookie(const std::string &name) const {
    auto pos = cookies.find(utils::to_lowercase_str(name));
    if (pos != cookies.end()) {
        return pos->second;
    }
    return std::nullopt;
}

[[nodiscard]]
std::unique_ptr<Json::Value> HttpRequest::body_as_json() const {
    const std::string raw_json = request.substr(request.find("\r\n\r\n") + 4);
    Json::Value json_obj;


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
    
    // Slash parsing
    // TODO: Fix when there is several slashes like /api/auth/{id}
    auto param_name_iter = param_names.begin(); // param_names stores id, user from api/{id}/{user} (example)
    size_t endpoint_start = request.find("/");
    std::string_view request_url{request.data() + endpoint_start + 1, request.find("HTTP") - endpoint_start - 2}; // additional 1 taking a space into account

    // Slash handling
    std::string_view endpoint_name_str{endpoint_name_str_};
    endpoint_name_str.remove_prefix(1);

    auto template_args = endpoint_name_str | std::views::split('/')
        | std::views::transform([](auto&& range) {
            return std::string{range.begin(), range.end()};
        }) | std::ranges::to<std::vector>();;
    auto args = request_url | std::views::split('/') 
        | std::views::transform([](auto&& range) {
        return std::string{range.begin(), range.end()};
    }) | std::ranges::to<std::vector>();

    if (template_args.size() != args.size()) {
        throw std::runtime_error("Template endpoint size != Endpoint size. Please check your endpoint name for errors");
    }

    for (size_t i = 0; i < args.size(); ++i) {
        if (i != 0 && i != args.size() - 1 && !args[i].empty() && template_args[i].contains('{')) { // Last is garbage value, first sometimes is empty or something else we dont need that
            if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
            parameters.emplace(*param_name_iter, args[i]);
            ++param_name_iter;
        }
    }
    if (args.size() == 1) { // If there is only 1 slash argument, it means there was no slash arguments, hence we can skip to the '?'
        auto question_pos = request_url.find('?');
        request_url.remove_prefix(question_pos);
    }  else { // Moving to the last slash argument
        request_url.remove_prefix(request_url.find_last_of("/") + 1);
        endpoint_name_str.remove_prefix(endpoint_name_str.find_last_of("/") + 1);
    }

    if (request_url.front() != '?') {
        if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
        auto endpoint_question_pos = endpoint_name_str.find('?');
        if (endpoint_name_str.substr(0, endpoint_question_pos).contains('{')) {
            parameters.emplace(*param_name_iter, request_url.substr(0, request_url.find('?')));
            ++param_name_iter;
        } 
        auto question_pos = request_url.find('?');
        request_url.remove_prefix(question_pos);
    }

    // Question mark parsing
    auto question_mark_pos = request_url.find("?");
    if (question_mark_pos != std::string::npos) { // ?name={smth} parsing 
        if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");

        size_t start_pos = question_mark_pos + 1;
        auto ampersand_pos = request_url.find("&");
        size_t end_pos = ampersand_pos != std::string::npos ? ampersand_pos : request_url.size();
        
        std::string_view key_value = request_url.substr(start_pos, end_pos - start_pos);
        auto value = key_value.substr(key_value.find("=") + 1);
        parameters.emplace(utils::to_lowercase_str(*param_name_iter), std::string{value}); 
        param_name_iter++; 
 
        request_url.remove_prefix(end_pos);   

        std::ranges::for_each(request_url | std::views::split('&') 
            | std::views::transform([](auto&& range) {
                std::string str{range.begin(), (size_t)std::ranges::distance(range)};
                return str | std::views::split('=') | std::views::transform([](auto&& range) {
                    return std::string{range.begin(), range.end()};
                }) | std::ranges::to<std::vector>();
            }), 
        [this](const auto& vec) {
            if (vec.size() == 2) {
                parameters.emplace(vec[0], vec[1]);
            }
        });

        // while ((ampersand_pos = request_url.find("&")) != std::string::npos) { // &name={smth} parsing
        //     if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
        //     size_t start_pos = ampersand_pos + 1;
        //     size_t end_pos = request_url.find("&", ampersand_pos + 1);
        //     end_pos = end_pos == std::string::npos ? request_url.size() : end_pos;
            
        //     std::string_view key_value = request_url.substr(start_pos, end_pos - start_pos);
        //     std::string_view value = key_value.substr(key_value.find("=") + 1);
            
        //     parameters.emplace(utils::to_lowercase_str(*param_name_iter), std::string{value});
        //     request_url.remove_prefix(end_pos == std::string::npos ? request_url.size() : end_pos);
        //     param_name_iter++;
        // }   
    }
}


void HttpRequest::extract_headers() {
    auto header_section_start = request.find("\r\n") + 2;
    auto header_section_end = request.find("\r\n\r\n");
    if (header_section_start == header_section_end) {
        return; // Headers are empty. Wtf?
    }
    std::istringstream strm(request.substr(header_section_start, header_section_end - header_section_start)); // here
    std::string line;
    while (std::getline(strm, line, '\n')) { // Extracting headers one by one
        utils::trim(line);

        auto column_pos = line.find(":");
        std::string name = line.substr(0, column_pos);
        auto value_start = line.find_first_not_of(" ", column_pos + 1);
        if (value_start == std::string::npos) {
            throw std::runtime_error("Malformed http request");
        }
        std::string value = line.substr(value_start); 
        
        if (utils::to_lowercase_str(name) != "cookie") {
            headers.emplace(utils::to_lowercase_str(name), std::move(value)); // Add header
        } else { // If it is a cookie
            std::string_view values_str{line.begin() + value_start, line.end()};
            while (!values_str.empty()) {
                auto semicolon_pos = values_str.find(";");
                auto eq_pos = values_str.find("="); 
                if (eq_pos == std::string::npos) {
                    throw std::runtime_error("Malformed http request");
                }

                std::string cookie_name = std::string{values_str.substr(0, eq_pos)};
                std::string value = (semicolon_pos == std::string::npos) 
                                    ? std::string{values_str.substr(eq_pos + 1)} 
                                    : std::string{values_str.substr(eq_pos + 1, semicolon_pos - eq_pos - 1)};
                utils::trim(cookie_name);
                utils::trim(value);
                cookies.emplace(utils::to_lowercase_str(cookie_name), Cookie{std::move(cookie_name), std::move(value)});
               
                values_str.remove_prefix(semicolon_pos == std::string::npos ? values_str.size() : semicolon_pos + 1);
            }
        }

    }
    
}