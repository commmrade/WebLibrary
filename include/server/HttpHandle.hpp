#pragma once

#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <functional>
#include "RequestType.hpp"
#include <span>
#include <vector>
#include "funcs.hpp"



class HttpHandle { // HttpHandle data class
public:
    HttpHandle() = default;
 

    void add_filter(Filter &&filter);
    void set_handle_method(Handler&& handle);
    void add_http_method(RequestType method);

    [[nodiscard]] 
    std::span<const Filter> get_filters() const {
        return filters;
    }
    [[nodiscard]] 
    std::span<const RequestType> get_methods() const {
        return methods;
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