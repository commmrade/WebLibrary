#include <expected>
#include <ostream>
#include <server/HttpRouter.hpp>
#include "server/HttpBinder.hpp"
#include "server/HttpHandle.hpp"
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include "server/RequestType.hpp"
#include <exception>
#include <server/Utils.hpp>
#include <server/hash.hpp>
#include <print>


std::expected<std::pair<std::string, std::string>, std::string> HttpRouter::parse_request_line(std::string_view request_string) {
    auto method_str = request_string.substr(0, request_string.find(" "));
    auto first_space = request_string.find(' ');
    auto second_space = request_string.find(' ', first_space + 1);
    if (first_space == std::string_view::npos || second_space == std::string_view::npos) {
        return std::unexpected{"Malformed request"};
    }
    std::string_view endpoint_target = request_string.substr(first_space + 1, second_space - first_space - 1);

    return std::pair{std::string{method_str}, std::string{endpoint_target}};
}

void HttpRouter::handle_request(HttpResponseWriter& resp, std::string_view path, std::string_view request_string, const std::string& processed_endpoint, std::string_view method, RequestType request_type) {
    try {
        const HttpHandle* handle = HttpBinder::instance().find_handle(processed_endpoint,path,request_type);
        // Do not destroy, since it is stored inside HttpBinder
        if (handle) {  
            HttpRequest request(false, std::string{request_string}, handle->get_endpoint_name_str(), handle->get_param_names()); // Passing param names to then process query part
            if (!handle->pass_middlewares(request)) {
                debug::log_warn("Filtering not passed");
                auto resp_ = HttpResponseBuilder()
                    .set_status(403)
                    .set_body("Access denied") // TODO: Universal error JSON response struct
                    .set_type(ContentType::TEXT)
                    .build();
                resp.respond(resp_);
                return;
            }
            debug::log_info("Proceeding to the endpoint ", processed_endpoint);
            (*handle)(request, resp); // Proceeding to endpoint if all middlewares were passed successfuly
        } else { // Endpoint was not found
            debug::log_info("Endpoint not found");
            auto resp_ = HttpResponseBuilder()
                .set_status(404)
                .set_body("Not found")
                .set_type(ContentType::TEXT)
                .build();
            resp.respond(resp_);
        }
    } catch (const std::exception &ex) { // Server internal error 5xx
        debug::log_error("Server internal error ", method, " ", ex.what());
        auto resp_ = HttpResponseBuilder()
            .set_status(500)
            .set_body("Server internal error")
            .set_type(ContentType::TEXT)
            .build();
        resp.respond(resp_);
    }
}

void HttpRouter::process_request(int client_socket, std::string_view request_string) {
    if (request_string.empty()) return;
    HttpResponseWriter resp{client_socket};
    auto _ = parse_request_line(request_string)
        .and_then([&](std::pair<std::string, std::string>&& method_path) {
            
            auto& [method, path] = method_path;
            std::string processed_endpoint = utils::process_url_str(path);
            RequestType request_type = utils::req_type_from_str(method);
            handle_request(resp, path, request_string, processed_endpoint, method, request_type);
            return std::expected<void, std::string>{};
        })
        .or_else([&](std::string&& error) {
             
            auto resp_ = HttpResponseBuilder()
                .set_status(400)
                .set_body(error)
                .set_type(ContentType::TEXT)
                .build();
            resp.respond(resp_);
            return std::expected<void, std::string>{};
        });
}