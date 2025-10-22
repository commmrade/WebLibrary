#include <optional>
#include "weblib/server/HttpRouter.hpp"
#include "weblib/server/HttpHandle.hpp"
#include "weblib/server/HttpRequest.hpp"
#include "weblib/server/HttpBinder.hpp"
#include <exception>
#include "weblib/server/RequestType.hpp"
#include "weblib/utils.hpp"
#include "weblib/server/hash.hpp"
namespace weblib
{

void HttpRouter::handle_request(HttpResponseWriter &resp, std::string_view path,
                                std::string_view raw_http, RequestType request_type)
{
    try
    {
        const HttpHandle *handle = HttpBinder::instance().find_handle(path, request_type);
        // Do not destroy, since it is stored inside HttpBinder
        if (handle != nullptr)
        {
            HttpRequest const req(std::string{raw_http}, handle->get_path(),
                                  handle->get_parameters());
            if (!handle->pass_middlewares(req))
            {
                debug::log_warn("Filtering not passed");
                auto res = HttpResponseBuilder()
                               .set_status(403)
                               .set_body_json(utils::error_response("Middleware", "Access denied"))
                               .set_content_type(ContentType::TEXT)
                               .build();
                resp.respond(res);
                return;
            }
            debug::log_info("Proceeding to the endpoint ");
            (*handle)(req, resp);
        }
        else
        {
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
    {
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
    HttpResponseWriter response{sock};
    auto [method, path] = utils::parse_request_line(raw_http);
    RequestType const request_type = req_type_from_str(method);
    
    handle_request(response, path, raw_http, request_type);
}
} // namespace weblib