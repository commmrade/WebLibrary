#pragma once

#include "server/HttpHandle.hpp"
#include "server/RequestType.hpp"
#include "server/Utils.hpp"
#include <ostream>
#include <unordered_map>
#include <string>
#include <print>
#include <ranges>

class HttpBinder {
private:
    std::unordered_map<std::string, HttpHandle> handles; 
public:
    static HttpBinder& instance() {
        static HttpBinder binder;
        return binder;
    }

    void register_handler(const std::string& endpoint_name, Handler &&handler, RequestType type) {
        auto handle = handles.find(endpoint_name);
        if (handle != handles.end()) {
            throw std::runtime_error("Endpoint is already set!");
        }    

        HttpHandle handle_obj{};
        handle_obj.add_http_method(type);
        handle_obj.set_handle_method(std::move(handler));
        handle_obj.set_param_names(utils::extract_params(endpoint_name)); // Extracting param names that are in {param_name}
        
        handles.emplace(utils::process_url_str(endpoint_name), std::move(handle_obj)); // Turning route to name={} kinda

        debug::log_info("Registered handler");
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

    const HttpHandle* find_handle(const std::string& endpoint, RequestType type) {
        auto handle = handles.find(endpoint);
        if (handle != handles.end() && handle->second.has_method(type)) {
            return &handle->second;
        }
        return nullptr;
    }
};