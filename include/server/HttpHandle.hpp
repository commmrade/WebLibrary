#pragma once

#include "server/HttpResponse.hpp"
#include <algorithm>
#include <functional>
#include "RequestType.hpp"
#include <span>
#include <vector>
#include "funcs.hpp"

class HttpRequest;

class HttpHandle { // HttpHandle data class
public:
    HttpHandle() = default;
 

    void add_filter(Filter &&filter);
    void set_handle_method(Handler&& handle);
    void add_http_method(RequestType method);

    bool pass_middlewares(const HttpRequest& request) const;
    bool has_method(RequestType type) {
        return std::ranges::any_of(methods, [type](auto _type) {
            return _type == type;
        });
    } 

    void set_param_names(std::vector<std::string>&& vec);
    [[nodiscard]]
    std::span<const std::string> get_param_names() const {
        return parameter_names;
    }

    void operator()(const HttpRequest& req, HttpResponseWriter& resp) const;

private:
    std::vector<RequestType> methods;
    std::vector<Filter> filters;
    std::vector<std::string> parameter_names;
    Handler handle;
};