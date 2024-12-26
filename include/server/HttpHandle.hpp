#pragma once

#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <functional>
#include "RequestType.hpp"
#include <stdexcept>
#include <vector>


using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;
using Filter = std::function<bool(const HttpRequest&)>;

class HttpHandle { // HttpHandle data class
public:
    HttpHandle() = default;
 

    void add_filter(Filter filter) {
        filters.push_back(std::move(filter));

    }

    void set_handle_method(Handler handle) {
        this->handle = std::move(handle);
    }

    void add_http_method(RequestType method) {
        methods.push_back(std::move(method));
    }

    [[nodiscard]]
    std::vector<Filter> get_filters() const {
        return filters;
    }

    [[nodiscard]]
    std::vector<RequestType> get_methods() const {
        return methods;
    }

    void proceed(const HttpRequest& req, HttpResponse& resp) {
        if (!handle) {
            throw std::runtime_error("Handle is not set");
        }
        handle(req, resp);
    }

private:
    std::vector<RequestType> methods;
    std::vector<Filter> filters;
    Handler handle;
};