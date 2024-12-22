#pragma once

#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <algorithm>
#include <exception>
#include <functional>
#include <stdexcept>
#include "Utils.hpp"
#include "hash.hpp"
#include "HttpHandle.hpp"



class HttpRouter {
public:
    HttpRouter() {

    }
    HttpRouter(const HttpRouter&) = delete;
    HttpRouter(HttpRouter &&) = delete;
    HttpRouter& operator=(const HttpRouter&) = delete;
    HttpRouter& operator=(HttpRouter&&) = delete;

    // template<typename... Types>
    void register_handler(const std::string &endpoint_name, Handler handler, RequestType type) {
        auto handle = handles.find(endpoint_name);

        if (handle != handles.end()) {
            throw std::runtime_error("Endpoint is already set!");
        }    
        HttpHandle handle_obj{};
        handle_obj.add_http_method(type);
        handle_obj.set_handle_method(handler);
        

        
        handles.emplace(endpoint_name, std::move(handle_obj));
    }

    template<typename... Types>
    void register_handler(const std::string &endpoint_name, Handler handler, Types... types) {
        auto handle = handles.find(endpoint_name);

        if (handle != handles.end()) {
            throw std::runtime_error("Endpoint is already set!");
        }    
        HttpHandle handle_obj{};
        (handle_obj.add_http_method(types), ...);
        handle_obj.set_handle_method(handler);
        

        handles[endpoint_name] = std::move(handle_obj);
    }

    
    void register_filter(const std::string &route, Filter filter) {
        auto handle = handles.find(route);
        if (handle == handles.end()) {
            throw std::runtime_error("Please, set controllers before filters");
        }

        handle->second.add_filter(filter);
    }

    static HttpRouter& instance() {
        static HttpRouter router{};

        return router;
    }


    void process_endpoint(int client_socket, const std::string &call) {
        std::string method = call.substr(0, call.find("/") - 1); // Extracting method from request
        
        RequestType request_type = req_type_from_str(method);
        
        std::string api_route = call.substr(call.find(" ") + 1, call.find("HTTP") - (call.find(" ") + 2)); // URL path that was called like /api/HttpServer
        std::string base_url = process_url_str(api_route); // Replacing queries with ?
       
        
    
        try {
            HttpResponse resp(client_socket);
            HttpRequest req(call);

            if (auto handle = handles.find(base_url); handle != handles.end() && std::ranges::contains(handle->second.get_methods(), request_type)) {
                auto middlewares = handle->second.get_filters();
                std::cout << handle->second.get_methods().size() << std::endl;
                
                for (auto &middleware : middlewares) { // Going through every middleware 
                    if (!middleware(req)) {
                        Response resp_{401, "Access denied", ResponseType::TEXT};
                        resp.respond(resp_);
                        
                        return;
                    }
                }

                handle->second.proceed(req, resp); // Proceeding to endpoint if all middlewares were passed successfuly

            } else { // Endpoint was not found
                Response rsp{404, "Not found", ResponseType::TEXT};
                resp.respond(rsp);
            }
        
        } catch (std::exception &ex) {
            std::cerr << "Exception in " << api_route << " or incorrectly formatted request" << std::endl;

            Response rsp{500, "Server internal error", ResponseType::TEXT};
            HttpResponse(client_socket).respond(rsp);
        }

    }

private:
    std::unordered_map<std::string, HttpHandle> handles; 
    



};