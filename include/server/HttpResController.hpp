#pragma once

#include <fstream>
#include <optional>
#include <sstream>
#include<stdarg.h>
#include <utility>
#include "server/HttpBinder.hpp"
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include "server/HttpRouter.hpp"
#include <debug.hpp>


class HttpResController {
public:
    HttpResController();

    HttpResController(const HttpResController&) = delete;
    HttpResController(HttpResController &&) = delete;
    HttpResController& operator=(const HttpResController&) = delete;
    HttpResController& operator=(HttpResController&&) = delete;
   
    template<typename... Values>
    static void register_method(Values... val) {
        HttpBinder::instance().register_handler(std::forward<Values>(val)...);
    }

    void process_file_request(const HttpRequest &req, HttpResponse &resp);

    std::optional<std::string> read_file(const std::string &filepath);
    std::string get_file_path(std::string &&req);
};


