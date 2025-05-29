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

HttpRequest::HttpRequest(bool hdrs_only_temp, const std::string &request_str, std::string endpoint_name_str, std::span<const std::string> pnames)
    : request(request_str), param_names(std::vector(pnames.begin(), pnames.end())), endpoint_name_str_(std::move(endpoint_name_str)) 
{
    extract_headers();
    if (!hdrs_only_temp) {
        extract_queries();
    } 
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
    
    auto param_name_iter = param_names.begin(); // param_names stores id, user from api/{id}/{user} (example)
    size_t endpoint_start = request.find("/");
    std::string_view request_url{request.data() + endpoint_start + 1, request.find("HTTP") - endpoint_start - 2}; // additional 1 taking a space into account
    // Slash handling
    std::string_view endpoint_name_str{endpoint_name_str_};
    endpoint_name_str.remove_prefix(1);

    auto template_args = endpoint_name_str | std::views::split('/') | std::ranges::to<std::vector<std::string>>();;
    auto args = request_url | std::views::split('/') | std::ranges::to<std::vector<std::string>>();;

    if (template_args.size() != args.size()) {
        throw std::runtime_error("Template endpoint size != Endpoint size. Please check your endpoint name for errors");
    }
    for (size_t i = 0; i < args.size(); ++i) {
        // std::println("{} {}", args[i], template_args[i]);
        if (i != 0 && i != args.size() - 1 && !args[i].empty() && template_args[i].contains('{')) { // Last is garbage value, first sometimes is empty or something else we dont need that
            if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
            parameters.emplace(*param_name_iter, args[i]);
            ++param_name_iter;
        }
    }
    if (args.size() == 1) { // If there is only 1 slash argument, it means there was no slash arguments, hence we can skip to the '?'
        auto question_pos = request_url.find('?');
        request_url.remove_prefix(question_pos == std::string::npos ? request_url.size() : question_pos);
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
        request_url.remove_prefix(question_pos == std::string::npos ? request_url.size() : question_pos);
    }

    // Question mark parsing
    auto question_mark_pos = request_url.find("?");
    if (question_mark_pos != std::string::npos) { // ?name={smth} parsing 
        if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");

        size_t start_pos = question_mark_pos + 1;
        auto ampersand_pos = request_url.find("&");
        size_t end_pos = ampersand_pos != std::string::npos ? ampersand_pos : request_url.size();
        
        auto value = request_url.substr(start_pos, end_pos - start_pos) 
            | std::views::split('=') 
            | std::views::drop(1) 
            | std::views::join 
            | std::ranges::to<std::string>();
        parameters.emplace(utils::to_lowercase_str(*param_name_iter), std::move(value)); 
        param_name_iter++; 
        request_url.remove_prefix(end_pos);   

        auto splitted_params = request_url 
            | std::views::split('&');
        std::ranges::for_each(splitted_params,
        [this](const auto& param) {
            auto key_value = param | std::views::split('=') | std::ranges::to<std::vector<std::string>>();
            if (key_value.size() == 2) {
                parameters.emplace(key_value[0], key_value[1]);
            }
        });
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

        // Construct vector with two parts: before and after first colon
        std::vector<std::string> name_value {
            std::string(line.begin(), line.begin() + pos),
            std::string(line.begin() + pos + 1, line.end())
        };
        auto name = std::move(name_value.front());
        auto value = std::move(name_value.back());
        utils::trim(value);
        if (name_value.size() != 2) {
            std::cerr << "Dist: " << name_value.size() << "\n";
            throw std::runtime_error("Malformed http request 1");
        }
        if (utils::to_lowercase_str(name) != "cookie") {
            headers.emplace(utils::to_lowercase_str(name), std::move(value)); // Add header
        } else { // If it is a cookie
            auto values = value | std::views::split(';') | std::views::transform([](auto&& range) {
                auto val = std::string{range.begin(), range.end()};
                utils::trim(val);
                return val;
            }) | std::ranges::to<std::vector<std::string>>();
            std::ranges::for_each(values, [this](auto&& cookie) {
                auto name_value = cookie | std::views::split('=') | std::ranges::to<std::vector<std::string>>();
                if (name_value.size() != 2) {
                    throw std::runtime_error("Malformed http request");
                }
                auto name = std::move(name_value.front());
                auto value = std::move(name_value.back());
                cookies.emplace(utils::to_lowercase_str(name), Cookie{std::move(name), std::move(value)});
            });
        }

    }
    
}