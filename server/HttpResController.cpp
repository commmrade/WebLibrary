#include <server/HttpResController.hpp>
#include <fstream>


HttpResController::HttpResController() {
    register_method("/static/public/", [this] (const HttpRequest &req, HttpResponse &resp) {process_file_request(req, resp); }, RequestType::GET, RequestType::OPTIONS);
    register_method("/static/private/", [this] (const HttpRequest &req, HttpResponse &resp) {process_file_request(req, resp); }, RequestType::GET, RequestType::OPTIONS);
    debug::log_info("Registered resource endpoints");
    // Apply filter to one of these endpoints to make resources protected
}


void HttpResController::process_file_request(const HttpRequest &req, HttpResponse &resp) {
    debug::log_info("Processing file request");

    auto full_path = get_file_path(req.get_raw());
    const std::string file_extension = full_path.substr(full_path.find_last_of(".") + 1);
    auto filename = full_path.substr(full_path.find_last_of("/") + 1);

    auto file_opt = read_file(full_path); 
    if (!file_opt) {
        debug::log_error("File not found");
        Response response{404, "Not found"};
        resp.respond(response);
    }

    Response response{200, file_opt.value()};
    if (file_extension == "css") {
        response.add_header(HeaderType::CONTENT_TYPE, "text/css");
    } else if (file_extension == "js") {
        response.add_header(HeaderType::CONTENT_TYPE, "text/javascript");
    } else if (file_extension == "html") {
        response.add_header(HeaderType::CONTENT_TYPE, "text/html");
    } else {
        response.add_header(HeaderType::CONTENT_TYPE, "plain/text");
    }
    
    resp.respond(response);
}

std::optional<std::string> HttpResController::read_file(const std::string &filepath) {
    std::ifstream file(filepath);

    if (!file.is_open()) {
        return std::nullopt;
    }
    std::stringstream ss;
    ss << file.rdbuf();

    return ss.str();
}

std::string HttpResController::get_file_path(std::string &&req) {
    const std::string req_str = req;
    const std::string filepath = req_str.substr(req_str.find("/"), req_str.find("HTTP") - req_str.find("/") - 1).substr(1);
    
    return "static/" + filepath;
}