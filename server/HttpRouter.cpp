#include <server/HttpRouter.hpp>
#include "server/HttpBinder.hpp"
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <algorithm>
#include <exception>
#include <functional>
#include <server/Utils.hpp>
#include <server/hash.hpp>

void HttpRouter::process_endpoint(int client_socket, std::string_view call) {
    RequestType request_type = utils::req_type_from_str(call.substr(0, call.find("/") - 1)); // Extracting method from request
    std::string_view api_route = call.substr(call.find(" ") + 1, call.find("HTTP") - (call.find(" ") + 2)); // URL path like /api/HttpServer
    std::string base_url = utils::process_url_str(api_route); // Replacing queries with {}

    try {
        HttpResponse resp(client_socket);
        
        if (is_file_url(base_url)) { 
            auto filename = base_url.substr(base_url.find_last_of("/") + 1);
            base_url = "/static/" + utils::find_file(filename);
        } // if it's a request for a file
        
        auto &binder = HttpBinder::instance();

        if (auto handle = binder.get_handles().find(base_url); handle != binder.get_handles().end() && std::ranges::contains(handle->second.get_methods(), request_type)) {
            HttpRequest req(std::string(call), handle->second.get_param_names()); // Passing param names to then process query part
            
            auto middlewares = handle->second.get_filters();    
            for (auto &middleware : middlewares) { // Going through every middleware 
                if (!middleware(req)) {
                    debug::log_warn("Filtering not passed");

                   
                    auto resp_ = ResponseBuilder()
                    .set_status(403)
                    .set_body("Access denied")
                    .set_type(ResponseType::TEXT)
                    .build();

                    resp.respond(resp_);
                    return; // Didn't pass a filter
                }
            }
            debug::log_info("Proceeding to the endpoint ", base_url);
            handle->second.proceed(req, resp); // Proceeding to endpoint if all middlewares were passed successfuly
        } else { // Endpoint was not found
            debug::log_info("Endpoint not found");

            auto resp_ = ResponseBuilder()
                    .set_status(404)
                    .set_body("Not found")
                    .set_type(ResponseType::TEXT)
                    .build();
            resp.respond(resp_);
        }
    
    } catch (const std::exception &ex) { // Server internal error 5xx
        debug::log_error("Server internal error ", api_route, " ", ex.what());
        std::cerr << "Exception in " << api_route << " or incorrectly formatted request " << ex.what() << std::endl;
        
        auto resp_ = ResponseBuilder()
                    .set_status(500)
                    .set_body("Server internal error")
                    .set_type(ResponseType::TEXT)
                    .build();
        HttpResponse(client_socket).respond(resp_);
    }
}

bool HttpRouter::is_file_url(std::string_view base_url) {
        if (auto dot_place = base_url.find_last_of("."); dot_place != std::string::npos && dot_place > base_url.find_last_of("/")) { 
        return true;
    } 
    return false;
}