#include "weblib/server/HttpHandle.hpp"
#include <algorithm>



void HttpHandle::add_filter(Filter &&filter) {
    m_filters.emplace_back(std::move(filter));
}

void HttpHandle::set_handle_method(Handler &&handle) {
    m_handle = std::move(handle);
}

void HttpHandle::add_http_method(RequestType method) {
    m_methods.emplace_back(method);
}

void HttpHandle::set_param_names(std::vector<std::string>&& vec) {
    m_parameter_names = std::move(vec);
}

void HttpHandle::operator()(const HttpRequest& req, HttpResponseWriter& resp) const {
    if (!m_handle) {
        throw std::runtime_error("Handle is not set");
    }
    m_handle(req, resp);
}

auto HttpHandle::pass_middlewares(const HttpRequest& request) const -> bool {
    return !std::ranges::any_of(m_filters, [&](auto&& filter) {
        return !filter(request);
    });
}


void HttpHandle::set_endpoint_name_str(std::string ep_name) {
    m_path = std::move(ep_name);
}