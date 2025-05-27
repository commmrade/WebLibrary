#pragma once

#include "server/HttpHandle.hpp"
#include "server/RequestType.hpp"
#include "server/Utils.hpp"
#include <ostream>
#include <string_view>
#include <unordered_map>
#include <string>
#include <print>
#include <ranges>
#include <regex>

class HttpBinder {
private:
    std::unordered_map<std::string, HttpHandle> handles; 
public:
    static HttpBinder& instance() {
        static HttpBinder binder;
        return binder;
    }

    template<typename... Types>
    void register_handler(const std::string& endpoint_name, Handler&& handler, Types&&... types) {
        auto handle = handles.find(endpoint_name);
        if (handle != handles.end()) {
            throw std::runtime_error("Endpoint is already set!");
        }    
        
        HttpHandle handle_obj{};
        (handle_obj.add_http_method(types), ...);
        handle_obj.set_handle_method(std::move(handler));      
        handle_obj.set_param_names(utils::extract_params(endpoint_name)); // Extracting param names that are in {param_name}
        handle_obj.set_endpoint_name_str(endpoint_name);
        std::println("Extract: {}", utils::extract_params(endpoint_name));
        handles.emplace(utils::process_url_str(endpoint_name), std::move(handle_obj)); // Turning route to name={} kinda
        debug::log_info("Registered handler");
    }

    
    void register_filter(std::string_view route, Filter&& filter) {
        auto handle = handles.find(utils::process_url_str(route));
        if (handle == handles.end()) {
            throw std::runtime_error("Please, set controllers before filters");
        }

        handle->second.add_filter(std::move(filter));
        debug::log_info("Registered filter");
    }

    const HttpHandle* find_handle(const std::string& endpoint, std::string_view path, RequestType type) {
    auto handle = handles.find(endpoint);
    if (handle == handles.end() || !handle->second.has_method(type)) {
        return nullptr;
    }

    auto endpoint_name = handle->second.get_endpoint_name_str();
    
    size_t path_query_pos = path.find('?');
    size_t endpoint_query_pos = endpoint_name.find('?');
    
    std::string_view path_slash = path.substr(0, path_query_pos);
    std::string_view endpoint_name_slash = std::string_view(endpoint_name).substr(0, endpoint_query_pos);
    
    if (endpoint_name.find('?') < endpoint_name.find('{')) {
        if (path_slash == endpoint_name_slash) { // Если слэш параметров до ? нет
            return &handle->second;
        }
    } else {
        std::string endpoint_name_normalized;
        endpoint_name_normalized.reserve(endpoint_name.size());
        size_t idx{0};
        while (idx < endpoint_name_slash.size()) {
            if (endpoint_name_slash[idx] == '{' && idx + 1 < endpoint_name_slash.size()) {
                auto close_bracket_pos = endpoint_name_slash.find('}', idx + 1);
                if (close_bracket_pos != std::string::npos) {
                    endpoint_name_normalized += "{}";
                    idx = close_bracket_pos + 1; // Выйти за пределы {...}
                    continue;
                }
            }
            endpoint_name_normalized += endpoint_name_slash[idx];
            ++idx;
        }
        
        std::string_view endpoint_slash = std::string_view(endpoint).substr(0, endpoint.find('?'));
        std::println("{} {}", endpoint_name_normalized, endpoint_slash);
        if (endpoint_slash == endpoint_name_normalized) {
            return &handle->second;
        }
    }
    
    return nullptr;
}
};