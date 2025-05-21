#include <server/HttpHandle.hpp>


void HttpHandle::add_filter(Filter &&filter) {
    filters.emplace_back(std::move(filter));
}

void HttpHandle::set_handle_method(Handler &&handle) {
    this->handle = std::move(handle);
}

void HttpHandle::add_http_method(RequestType method) {
    methods.emplace_back(method);
}

void HttpHandle::set_param_names(std::vector<std::string>&& vec) {
    parameter_names = std::move(vec);
}

void HttpHandle::operator()(const HttpRequest& req, HttpResponseWriter& resp) const {
    if (!handle) {
        throw std::runtime_error("Handle is not set");
    }
    handle(req, resp);
}
