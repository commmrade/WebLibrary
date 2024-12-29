#pragma once

#include <fstream>
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
 
        const std::string req_str = req.get_raw();
        const std::string filepath = req_str.substr(req_str.find("/"), req_str.find("HTTP") - req_str.find("/") - 1).substr(1);
        const std::string full_path = "static/" + filepath;

        auto filename = filepath.substr(filepath.find_last_of("/") + 1);
        const std::string file_extension = filename.substr(filename.find_last_of(".") + 1);

        std::ifstream file(full_path);

        if (!file.is_open()) {
            Response response{404, "Not found"};
            resp.respond(response);
        }

        std::stringstream ss;
        ss << file.rdbuf();

        Response response{200, ss.str()};

        if (file_extension == "css") {
            response.add_header(HeaderType::CONTENT_TYPE, "text/css");
        } else if (file_extension == "js") {
            response.add_header(HeaderType::CONTENT_TYPE, "text/javascript");
        }
        resp.respond(response);
    }

    

};


