#include <ostream>
#include <server/HttpRouter.hpp>
#include "server/HttpBinder.hpp"
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <algorithm>
#include <exception>
#include <functional>
#include <server/Utils.hpp>
#include <server/hash.hpp>
#include <print>

void HttpRouter::process_endpoint(int client_socket, std::string_view request_string) {
    RequestType request_type = utils::req_type_from_str(request_string.substr(0, request_string.find("/") - 1)); // Extracting method from request TODO: FIX
    std::string_view api_route = request_string.substr(request_string.find(" ") + 1, request_string.find("HTTP") - (request_string.find(" ") + 2)); // URL path like /api/HttpServer TODO: MAKE CLEARER
    std::string base_url = utils::process_url_str(api_route); // Replacing queries with {}
    try {
        HttpResponseWriter resp(client_socket);
        
        auto &binder = HttpBinder::instance();

        // TODO: MAKE ALL THIS SHIT CLEANER
        if (auto handle = binder.get_handles().find(base_url); handle != binder.get_handles().end() && std::ranges::contains(handle->second.get_methods(), request_type)) {
            HttpRequest req(std::string(request_string), handle->second.get_param_names()); // Passing param names to then process query part
            
            auto middlewares = handle->second.get_filters();    
            for (auto &middleware : middlewares) { // Going through every middleware 
                if (!middleware(req)) {
                    debug::log_warn("Filtering not passed");
                    auto resp_ = HttpResponseBuilder()
                        .set_status(403)
                        .set_body("Access denied")
                        .set_type(ResponseType::TEXT)
                        .build();
                    resp.respond(resp_);
                    return; // Didn't pass a filter
                }
            }
            debug::log_info("Proceeding to the endpoint ", base_url);
            handle->second(req, resp); // Proceeding to endpoint if all middlewares were passed successfuly
        } else { // Endpoint was not found
            debug::log_info("Endpoint not found");
            auto resp_ = HttpResponseBuilder()
                .set_status(404)
                .set_body("Not found")
                .set_type(ResponseType::TEXT)
                .build();
            resp.respond(resp_);
        }
    
    } catch (const std::exception &ex) { // Server internal error 5xx
        debug::log_error("Server internal error ", api_route, " ", ex.what());
        std::cerr << "Exception in " << api_route << " or incorrectly formatted request " << ex.what() << std::endl;
        
        auto resp_ = HttpResponseBuilder()
                    .set_status(500)
                    .set_body("Server internal error")
                    .set_type(ResponseType::TEXT)
                    .build();
        HttpResponseWriter(client_socket).respond(resp_);
    }
}