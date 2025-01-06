#include <server/HttpResponse.hpp>

Response::Response(ResponseType type) {
    set_type(type);
}

Response::Response(int status_code, const std::string &resp_text, ResponseType type) : Response(type) {
    set_body(resp_text);
    set_status(status_code);
}

std::string Response::respond_text() const {
    std::string response{};
    std::string status_code_str = std::to_string(status_code);
    
    response += "HTTP/" + http_version + " " + status_code_str + " " + status_message + "\r\n";

    // Adding headers part
    for (const auto &[header_name, header_value] : headers) {
        response += header_name + ": " + header_value + "\r\n";
    }
    if (headers.find("Content-Length") == headers.end()) { // Setting Content-Length if not already set
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    }

    response += "\r\n"; // Separating headers and answer part

    response += body; // Adding user text

    return response;
}


void Response::add_header_raw(std::string name, std::string value) {
    if (name == "Set-Cookie") {
        throw std::runtime_error("Use add_cookie() instead!");
    }

    headers[name] = value;
}

void Response::add_cookie(const Cookie &cookie) {
    std::string cookie_str = cookie.get_string();
    // Maybe add multiple
    headers["Set-Cookie"] = cookie_str;
}

void Response::add_header(HeaderType header_type, std::string value) {
    switch (header_type) {
        case HeaderType::CONTENT_TYPE: {
            headers["Content-Type"] = value;
            break;
        }
        case HeaderType::CONTENT_LENGTH: {
            headers["Content-Length"] = value;
            break;
        }
        case HeaderType::AUTH_BASIC: {
            headers["Authorization"] = "Basic " + value;
            break;
        }
        case HeaderType::AUTH_BEARER: {
            headers["Authorization"] = "Bearer " + value;
            break;
        }
        default: {
            throw std::runtime_error("Header type is not implemented > todo!");
            break;
        }
    }
}

void Response::remove_header(const std::string &name) {
    headers.erase(name); // Removing if exists (doesn't throw if does not exist)
}

void Response::set_type(ResponseType type) {
    switch (type) {
        case ResponseType::HTML: {
            headers["Content-Type"] = "text/html";
            break;
        }   
        case ResponseType::JSON: {
            headers["Content-Type"] = "application/json";
            break;
        }
        case ResponseType::TEXT: {
            // Deliberately made without break
        }
        default: {
            headers["Content-Type"] = "plain/text";
            break;
        }
    }
}

[[nodiscard]]
std::optional<std::string> Response::get_header(const std::string &name) const {
    if (auto hdr = headers.find(name); hdr != headers.end()) {
        return hdr->second;
    }
    return std::nullopt;
}


void Response::set_body(const Json::Value& json_obj) {
    Json::FastWriter json_writer;
    const std::string json_str = json_writer.write(json_obj);
    

    body = json_str;
}

void Response::set_status(int status_code) {
    
    switch (status_code) {
        case 200: {
            status_message = "OK";
            break;
        }
        case 400: {
            status_message = "Bad request";
            break;
        }
        case 401: {
            status_message = "Unauthorized";
            break;
        }
        case 404: {
            status_message = "Not found";
            break;
        }
        case 500: {
            status_message = "Internal server error";
            break;
        }
        default: {
            status_message = "OK";
            break;
        }

    }
    this->status_code = status_code;
}


void HttpResponse::respond(Response &resp) { // Sending response text to the requester
    auto response = resp.respond_text();

    ssize_t bytes_sent = send(client_socket, response.c_str(), response.size(), 0);
    if (bytes_sent < 0) {
        debug::log_error("Error sending response");
    } 
}