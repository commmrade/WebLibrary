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

    void set_param_names(std::vector<std::string> vec) {
        parameter_names = std::move(vec);
    }


    [[nodiscard]]
    std::vector<std::string> get_param_names() const {
        return parameter_names;
    }

private:
    std::vector<RequestType> methods;
    std::vector<Filter> filters;
    std::vector<std::string> parameter_names;
    Handler handle;
};