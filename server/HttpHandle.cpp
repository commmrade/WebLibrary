#include "weblib/server/HttpHandle.hpp"
#include <algorithm>
#include "weblib/exceptions.hpp"

void HttpHandle::add_filter(Filter &&filter) { m_filters.emplace_back(std::move(filter)); }

void HttpHandle::set_handle_method(Handler &&handle) { m_handle = std::move(handle); }

void HttpHandle::add_http_method(RequestType method) { m_methods.emplace_back(method); }

void HttpHandle::set_parameters(std::vector<std::string> &&vec)
{
    m_parameter_names = std::move(vec);
}

void HttpHandle::operator()(const HttpRequest &req, HttpResponseWriter &resp) const
{
    if (!m_handle)
    {
        throw handle_not_set{};
    }
    m_handle(req, resp);
}

auto HttpHandle::pass_middlewares(const HttpRequest &request) const -> bool
{
    return !std::ranges::any_of(m_filters, [&](auto &&filter) { return !filter(request); });
}

void HttpHandle::set_path(std::string ep_name) { m_path = std::move(ep_name); }
