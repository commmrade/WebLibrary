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
        
        if (handle != handles.end() && handle->second.has_method(type)) {
            auto endpoint_name = handle->second.get_endpoint_name_str();
            if (endpoint_name.find('?') < endpoint_name.find('{')) { // Если аргументов до ? нет, значит можно сравнить части до ? и все
                auto path_slash = path.substr(0, path.find('?'));
                auto endpoint_slash = endpoint_name.substr(0, endpoint_name.find('?'));
                if (path_slash == endpoint_slash) return &handle->second;
            }
            // Если аргументы до ? есть, значит нужно сравнить endpoint name и обработанный endpoint (endpoint), если они совпадают после  реплейса у endpoint_name, значит одно и то же => proceed
            std::regex re(R"(\{[^}]*\})"); // match '{' followed by anything but '}' then '}'
            auto replace_with_braces = [&](std::string& s) {
                s = std::regex_replace(s, re, "{}");
            };
            auto endpoint_slash = endpoint.substr(0, endpoint.find('?'));
            auto endpoint_name_slash = endpoint_name.substr(0, endpoint_name.find('?'));
            replace_with_braces(endpoint_name_slash);
            if (endpoint_slash == endpoint_name_slash) return &handle->second;
        }
        return nullptr;
    }
};