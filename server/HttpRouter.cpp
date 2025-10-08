#include <optional>
#include "weblib/server/HttpRouter.hpp"
#include "weblib/server/HttpHandle.hpp"
#include "weblib/server/HttpRequest.hpp"
#include "weblib/server/HttpBinder.hpp"
#include <expected>
#include <exception>
#include "weblib/server/Utils.hpp"
#include "weblib/server/hash.hpp"
#include <print>

auto HttpRouter::parse_request_line(std::string_view request_string) -> std::optional<std::pair<std::string, std::string>> {
    auto method_str = request_string.substr(0, request_string.find(' '));
    auto first_space = request_string.find(' ');
    auto second_space = request_string.find(' ', first_space + 1);
    if (first_space == std::string_view::npos || second_space == std::string_view::npos) {
        return std::nullopt;
    }
    std::string_view const endpoint_target = request_string.substr(first_space + 1, second_space - first_space - 1);

    return std::optional<std::pair<std::string, std::string>>{std::pair{std::string{method_str}, std::string{endpoint_target}}};
}

void HttpRouter::handle_request(HttpResponseWriter& resp, std::string_view path, std::string_view request_string, std::string_view method, RequestType request_type) {
    try {
        const HttpHandle* handle = HttpBinder::instance().find_handle(path,request_type);
        // Do not destroy, since it is stored inside HttpBinder
        if (handle != nullptr) {
            HttpRequest const request(std::string{request_string}, handle->get_path(), handle->get_param_names()); // Passing param names to then process query part
            if (!handle->pass_middlewares(request)) {
                debug::log_warn("Filtering not passed");
                auto resp_ = HttpResponseBuilder()
                    .set_status(403)
                    .set_body_json(utils::error_response("Middleware", "Access denied"))
                    .set_content_type(ContentType::TEXT)
                    .build();
                resp.respond(resp_);
                return;
            }
            debug::log_info("Proceeding to the endpoint ");
            (*handle)(request, resp); // Proceeding to endpoint if all middlewares were passed successfuly
        } else { // Endpoint was not found
            debug::log_info("Endpoint not found");
            auto resp_ = HttpResponseBuilder()
                .set_status(404)
                // .set_body_str("Not found")
                .set_body_json(utils::error_response("Not found", "No such handle"))
                .set_content_type(ContentType::TEXT)
                .build();
            resp.respond(resp_);
        }
    } catch (const std::exception &ex) { // Server internal error 5xx
        debug::log_error("Server internal error ", method, " ", ex.what());
        auto resp_ = HttpResponseBuilder()
            .set_status(500)
            .set_body_json(utils::error_response("Internal error", std::format("Server internal error: {}", ex.what())))
            .set_content_type(ContentType::TEXT)
            .build();
        resp.respond(resp_);
    }
}

void HttpRouter::process_request(int client_socket, std::string_view request_string) {
    if (request_string.empty()) {
        return;
    }
    HttpResponseWriter resp{client_socket};


    auto val = HttpRouter::parse_request_line(request_string);
    if (!val.has_value()) {
        auto resp_ = HttpResponseBuilder()
            .set_status(400)
            .set_body_str("Malformed request")
            .set_content_type(ContentType::TEXT)
            .build();
        resp.respond(resp_);
    }
    auto& [method, path] = val.value();
    RequestType const request_type = req_type_from_str(method);
    handle_request(resp, path, request_string, method, request_type);
}
