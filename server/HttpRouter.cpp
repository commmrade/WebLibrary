#include <optional>
#include "weblib/server/HttpRouter.hpp"
#include "weblib/server/HttpHandle.hpp"
#include "weblib/server/HttpRequest.hpp"
#include "weblib/server/HttpBinder.hpp"
#include <exception>
#include "weblib/server/RequestType.hpp"
#include "weblib/server/Utils.hpp"
#include "weblib/server/hash.hpp"

auto HttpRouter::parse_request_line(std::string_view raw_http)
    -> std::optional<std::pair<std::string, std::string>>
{
    auto space_after_method  = raw_http.find(' ');
    if (space_after_method == std::string_view::npos)
    {
        return std::nullopt;
    }
    auto method   = raw_http.substr(0, space_after_method);
    
    auto space_after_path = raw_http.find(' ', space_after_method + 1);
    if (space_after_path == std::string_view::npos)
    {
        return std::nullopt;
    }

    std::string_view const path =
        raw_http.substr(space_after_method + 1, space_after_path - space_after_method - 1);
    return {
        std::pair{std::string{method}, std::string{path}}
    };
}

void HttpRouter::handle_request(HttpResponseWriter &resp, std::string_view path,
                                std::string_view raw_http,
                                RequestType request_type)
{
    try
    {
        const HttpHandle *handle = HttpBinder::instance().find_handle(path, request_type);
        // Do not destroy, since it is stored inside HttpBinder
        if (handle != nullptr)
        {
            HttpRequest const req(
                std::string{raw_http}, handle->get_path(),
                handle->get_parameters()); // Passing param names to then process query part
            if (!handle->pass_middlewares(req))
            {
                debug::log_warn("Filtering not passed");
                auto res =
                    HttpResponseBuilder()
                        .set_status(403)
                        .set_body_json(utils::error_response("Middleware", "Access denied"))
                        .set_content_type(ContentType::TEXT)
                        .build();
                resp.respond(res);
                return;
            }
            debug::log_info("Proceeding to the endpoint ");
            (*handle)(req,
                      resp); // Proceeding to endpoint if all middlewares were passed successfuly
        }
        else
        { // Endpoint was not found
            debug::log_info("Endpoint not found");
            auto res = HttpResponseBuilder()
                             .set_status(404)
                             // .set_body_str("Not found")
                             .set_body_json(utils::error_response("Not found", "No such handle"))
                             .set_content_type(ContentType::TEXT)
                             .build();
            resp.respond(res);
        }
    }
    catch (const std::exception &ex)
    { // Server internal error 5xx
        debug::log_error("Server internal error ", req_type_to_str(request_type), " ", ex.what());
        auto resp_ = HttpResponseBuilder()
                         .set_status(500)
                         .set_body_json(utils::error_response(
                             "Internal error", std::format("Server internal error: {}", ex.what())))
                         .set_content_type(ContentType::TEXT)
                         .build();
        resp.respond(resp_);
    }
}

void HttpRouter::process_request(int sock, std::string_view raw_http)
{
    if (raw_http.empty())
    {
        return;
    }

    HttpResponseWriter response{sock};
    auto method_and_path_opt = HttpRouter::parse_request_line(raw_http);
    if (!method_and_path_opt.has_value())
    {
        auto res = HttpResponseBuilder()
                         .set_status(400)
                         .set_body_str("Malformed request")
                         .set_content_type(ContentType::TEXT)
                         .build();
        response.respond(res);
    }
    auto &[method, path]           = method_and_path_opt.value();
    RequestType const request_type = req_type_from_str(method);
    handle_request(response, path, raw_http, request_type);
}