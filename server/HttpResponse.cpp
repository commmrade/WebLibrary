#include "debug.hpp"
#include <cerrno>
#include <format>
#include <server/HttpResponse.hpp>
#include <print>
#include <stdexcept>
#include <sys/poll.h>

std::string HttpResponse::respond_text() const {
    std::string response = std::format("HTTP/{} {} {}\r\n", http_version, status_code, status_message);
    // Adding headers part
    for (const auto &[header_name, header_value] : headers) {
        response += header_name + ": " + header_value + "\r\n";
    }
    if (headers.find("Content-Length") == headers.end()) { // Setting Content-Length if not already set
        response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    }

    response += "\r\n"; // Separating headers and answer par
    response += body; // Adding user text

    return response;
}


void HttpResponse::add_header_raw(const std::string& name, std::string_view value) {
    if (name == "Set-Cookie") {
        throw std::runtime_error("Use add_cookie() instead!");
    }

    headers[name] = value;
}

void HttpResponse::add_cookie(const Cookie &cookie) {
    std::string cookie_str = cookie.to_string();
    headers["Set-Cookie"] = cookie_str;
}

void HttpResponse::add_header(HeaderType header_type, std::string value) {
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

void HttpResponse::remove_header(const std::string &name) {
    headers.erase(name); // Removing if exists (doesn't throw if does not exist)
}

void HttpResponse::set_type(ResponseType type) {
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
            headers["Content-Type"] = "text/plain";
            break;
        }
    }
}

std::optional<std::string> HttpResponse::get_header(const std::string &name) const {
    if (auto hdr = headers.find(name); hdr != headers.end()) {
        return hdr->second;
    }
    return std::nullopt;
}


void HttpResponse::set_body(const Json::Value& json_obj) {
    Json::FastWriter json_writer;
    const std::string json_str = json_writer.write(json_obj);
    body = json_str;
}

void HttpResponse::set_status(int status_code) {
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
            status_message = "Unknown status code, set msg manually";
            break;
        }

    }
    this->status_code = status_code;
}


void HttpResponseWriter::respond(HttpResponse &resp) { // Sending response text to the requester
    auto response = resp.respond_text();
    size_t write_total_size = 0;

    while (write_total_size < response.size()) {
        ssize_t bytes_sent = send(client_socket, response.c_str() + write_total_size, response.size() - write_total_size, 0);
        if (bytes_sent < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            pollfd client;
            client.fd = client_socket;
            client.events = POLLOUT;

            int poll_result = poll(&client, 1, 5000);
            if (poll_result <= 0) {
                debug::log_error("Connection is lost");
                return;
            }
            continue;
        } else if (bytes_sent < 0) {
            debug::log_error("Sending failed");
            throw std::runtime_error("Sending failed");
        }
        write_total_size += bytes_sent;
    }
}

