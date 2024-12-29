#pragma once

#include <fstream>
#include <optional>
#include <sstream>
#include<stdarg.h>
#include <utility>
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"
#include "server/HttpRouter.hpp"

class HttpResController {
public:
    HttpResController() {
        register_method("/static/public/", [this] (const HttpRequest &req, HttpResponse &resp) {process_file_request(req, resp); }, RequestType::GET, RequestType::OPTIONS);
        register_method("/static/private/", [this] (const HttpRequest &req, HttpResponse &resp) {process_file_request(req, resp); }, RequestType::GET, RequestType::OPTIONS);

        // Apply filter to one of these endpoints to make resources protected
    }

    HttpResController(const HttpResController&) = delete;
    HttpResController(HttpResController &&) = delete;
    HttpResController& operator=(const HttpResController&) = delete;
    HttpResController& operator=(HttpResController&&) = delete;


   
    template<typename... Values>
    static void register_method(Values... val) {
        HttpRouter::instance().register_handler(std::forward<Values>(val)...);
    }

    void process_file_request(const HttpRequest &req, HttpResponse &resp) {

        auto full_path = get_file_path(req.get_raw());
        const std::string file_extension = full_path.substr(full_path.find_last_of(".") + 1);
        auto filename = full_path.substr(full_path.find_last_of("/") + 1);

        auto file_opt = read_file(full_path);
        if (!file_opt) {
            Response response{404, "Not found"};
            resp.respond(response);
        }

        Response response{200, file_opt.value()};
        if (file_extension == "css") {
            response.add_header(HeaderType::CONTENT_TYPE, "text/css");
        } else if (file_extension == "js") {
            response.add_header(HeaderType::CONTENT_TYPE, "text/javascript");
        }
        resp.respond(response);
    }

    std::optional<std::string> read_file(const std::string &filepath) {
        std::ifstream file(filepath);

        if (!file.is_open()) {
            return std::nullopt;
        }

        std::stringstream ss;
        ss << file.rdbuf();

        return ss.str();
    }

    std::string get_file_path(std::string &&req) {
        const std::string req_str = req;
        const std::string filepath = req_str.substr(req_str.find("/"), req_str.find("HTTP") - req_str.find("/") - 1).substr(1);
     
        return "static/" + filepath;
    }

};


