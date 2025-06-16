#pragma once

#include "server/HttpResponse.hpp"
#include <algorithm>
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

    [[nodiscard]] auto pass_middlewares(const HttpRequest& request) const -> bool;
    auto has_method(RequestType type) -> bool {
        return std::ranges::any_of(m_methods, [type](auto _type) {
            return _type == type;
        });
    } 

    void set_param_names(std::vector<std::string>&& vec);
    [[nodiscard]]
    auto get_param_names() const -> std::span<const std::string> {
        return m_parameter_names;
    }

    void set_endpoint_name_str(std::string ep_name);
    [[nodiscard]] auto get_endpoint_name_str() const -> std::string { return m_endpoint_name_str; }

    void operator()(const HttpRequest& req, HttpResponseWriter& resp) const;

private:
    std::vector<RequestType> m_methods;
    std::vector<Filter> m_filters;
    std::vector<std::string> m_parameter_names;
    std::string m_endpoint_name_str;
    Handler m_handle;
};