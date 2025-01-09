#pragma once

#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include <functional>
#include "RequestType.hpp"
#include <span>
#include <vector>


using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;
using Filter = std::function<bool(const HttpRequest&)>;

class HttpHandle { // HttpHandle data class
public:
    HttpHandle() = default;
 

    void add_filter(Filter &&filter);

    void set_handle_method(Handler handle);

    void add_http_method(RequestType method);

    std::vector<Filter> get_filters() const {
        return filters;
    }

    std::span<const RequestType> get_methods() const {
        return methods;
    }

    void proceed(const HttpRequest& req, HttpResponse& resp) const;

    void set_param_names(std::vector<std::string> vec);

    [[nodiscard]]
    std::span<const std::string> get_param_names() const {
        return parameter_names;
    }

private:
    std::vector<RequestType> methods;
    std::vector<Filter> filters;
    std::vector<std::string> parameter_names;
    Handler handle;
};