// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include "server/HttpHandle.hpp"
#include "server/RequestType.hpp"
#include "server/Utils.hpp"
#include <algorithm>
#include <string_view>
#include <unordered_map>
#include <string>
#include <print>
#include <ranges>
#include <utility>
#include <vector>

class HttpBinder {
private:
    std::unordered_map<std::string, HttpHandle> m_handles; 
public:
    static auto instance() -> HttpBinder& {
        static HttpBinder binder;
        return binder;
    }

    template<typename... Types>
    void register_handler(const std::string& endpoint_name, Handler&& handler, Types&&... types) {
        auto handle = m_handles.find(endpoint_name);
        if (handle != m_handles.end()) {
            throw std::runtime_error("Endpoint is already set!");
        }    
        
        HttpHandle handle_obj{};
        (handle_obj.add_http_method(std::forward<Types>(types)), ...);
        handle_obj.set_handle_method(std::move(handler));      
        handle_obj.set_param_names(utils::extract_params(endpoint_name)); // Extracting param names that are in {param_name}
        handle_obj.set_endpoint_name_str(endpoint_name);
        // std::println("Extract: {}", utils::extract_params(endpoint_name));
        m_handles.emplace(utils::process_url_str(endpoint_name), std::move(handle_obj)); // Turning route to name={} kinda
        debug::log_info("Registered handler");
    }

    
    void register_filter(std::string_view route, Filter&& filter) {
        const auto handle = m_handles.find(utils::process_url_str(route));
        if (handle == m_handles.end()) {
            throw std::runtime_error("Please, set controllers before filters");
        }

        handle->second.add_filter(std::move(filter));
        debug::log_info("Registered filter");
    }
    static auto match_path(std::string_view pattern, std::string_view path) -> bool {
        auto split_path = [](std::string_view str) -> std::vector<std::string_view> {
            return str | std::views::split('/') | std::views::transform([](auto&& range) {
                return std::string_view{range.begin(), static_cast<size_t>(std::distance(range.begin(), range.end()))};
            }) | std::views::filter([](auto&& inside_str) {
                return !inside_str.empty();
            }) | std::ranges::to<std::vector>();
        };

        auto parse_query = [](std::string_view query) -> std::unordered_map<std::string_view, std::string_view> {
            std::unordered_map<std::string_view, std::string_view> params;
            auto key_value_pairs = query 
                | std::views::split('&') 
                | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
                | std::ranges::to<std::vector>();
            
            std::ranges::for_each(key_value_pairs, [&](auto&& key_value_pair) {
                auto key_value = key_value_pair 
                    | std::views::split('=') 
                    | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
                    | std::ranges::to<std::vector>();
                params.emplace(key_value.front(), key_value.back());
            });
            return params;
        };

        // Разделяем path и query
        size_t const pattern_question_pos = pattern.find('?', 0);
        size_t const path_question_pos = path.find('?', 0);
        std::string_view const pattern_path = pattern.substr(0, pattern_question_pos);
        std::string_view const path_path = path.substr(0, path_question_pos);

        auto pattern_parts = split_path(pattern_path);
        auto path_parts = split_path(path_path);

        if (pattern_parts.size() != path_parts.size()) {
            return false;
        }
        for (const auto&& [pp, rp] : std::views::zip(pattern_parts, path_parts)) {
            if (pp == "{}") {
                continue;
            }
            if (pp != rp) {
                return false;
            }
        }

        if (pattern_question_pos != std::string_view::npos) {
            std::string_view const pattern_query = pattern.substr(pattern_question_pos + 1);
            std::string_view const path_query = (path_question_pos != std::string_view::npos) ? path.substr(path_question_pos + 1) : "";
            if (path_query.empty()) {
                return false;
            }

            auto pattern_params = parse_query(pattern_query);
            auto path_params = parse_query(path_query);
        
            if (pattern_params.size() != path_params.size()) {
                return false;
            }
            for (const auto& [key, value] : pattern_params) {
                if (!path_params.contains(key)) {
                    return false;
                }
                if (value != "{}" && path_params[key] != value) {
                    return false;
                }
            }
        }

        return true;
    }


    auto find_handle(std::string_view path, RequestType type) -> const HttpHandle* {
        for (auto& [pattern, handle] : m_handles) {
            if (!handle.has_method(type)) {
                continue;
            }
            if (match_path(pattern, path)) {
                return &handle;
            }
        }
        return nullptr;
    }

};