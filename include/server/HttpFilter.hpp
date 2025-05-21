#pragma once
#include "server/HttpBinder.hpp"
#include "server/HttpRequest.hpp"
#include <server/HttpServer.hpp>
#include <server/HttpRouter.hpp>
#include <debug.hpp>

#define mv(X) std::move(X)
#define REG_FILTER(NAME, FUNCTION) HttpFilter::register_filter(NAME, [this] (const HttpRequest &req) { return FUNCTION((req)); }) // Macro to register filter

template<typename Derived>
class HttpFilter {
public:
    HttpFilter() = default;
    HttpFilter(const HttpFilter&) = delete;
    HttpFilter(HttpFilter &&) = delete;
    HttpFilter& operator=(const HttpFilter&) = delete;
    HttpFilter& operator=(HttpFilter&&) = delete;

    bool doFilter(const HttpRequest &req) {
        return static_cast<Derived*>(this)->doFilter(req);
    }

    static void register_filter(std::string_view route, Filter filter) {
        debug::log_info("Registering a filter");
        HttpBinder::instance().register_filter(route, filter);
    }

};
