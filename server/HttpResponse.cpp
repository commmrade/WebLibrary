#include "debug.hpp"
#include <cerrno>
#include <format>
#include <server/HttpResponse.hpp>
#include <print>
#include <stdexcept>
#include <string_view>
#include <sys/poll.h>

std::string HttpResponse::to_string() const {
    std::string response = std::format("HTTP/{} {} {}\r\n", http_version, status_code, status_message);
    // Adding headers part
    for (const auto &[header_name, header_value] : headers) {
        response += std::format("{}: {}\r\n", header_name, header_value);
    }
    if (headers.find("Content-Length") == headers.end()) { // Setting Content-Length if not already set
        response += std::format("Content-Length: {}\r\n", body.size());
    }
    response += "\r\n"; // Separating headers and answer par
    response += body; // Adding user text
    return response;
}


void HttpResponse::set_header_raw(const std::string& name, std::string_view value) {
    if (name == "Set-Cookie") {
        throw std::runtime_error("Use add_cookie() instead!");
    }
    headers[name] = value;
}

void HttpResponse::set_cookie(const Cookie &cookie) {
    std::string cookie_str = cookie.to_string();
    headers["Set-Cookie"] = cookie_str;
}

void HttpResponse::set_header(HeaderType header_type, std::string_view value) {
    switch (header_type) {
        case HeaderType::CONTENT_TYPE: {
            headers["Content-Type"] = value;
            break;
        }
        case HeaderType::CONTENT_LENGTH: {
            headers["Content-Length"] = value;
            break;
        }
        case HeaderType::CACHE_CONTROL: {
            headers["Cache-Control"] = value;
            break;
        }
        case HeaderType::EXPIRES: {
            headers["Expires"] = value;
            break;
        }
        case HeaderType::SET_COOKIE: {
            headers["Set-Cookie"] = value;
            break;
        }
        case HeaderType::LOCATION: {
            headers["Location"] = value;
            break;
        }
        case HeaderType::SERVER: {
            headers["Server"] = value;
            break;
        }
        case HeaderType::ACCEPT: {
            headers["Accept"] = value;
            break;
        }
        case HeaderType::USER_AGENT: {
            headers["User-Agent"] = value;
            break;
        }
        case HeaderType::HOST: {
            headers["Host"] = value;
            break;
        }
        case HeaderType::ACCEPT_LANGUAGE: {
            headers["Accept-Language"] = value;
            break;
        }
        default: {
            throw std::runtime_error("Header type is not implemented > todo!");
        }
    }
}

void HttpResponse::remove_header(const std::string& name) {
    headers.erase(name); // Removing if exists (doesn't throw if does not exist)
}  

void HttpResponse::set_content_type(ContentType type) {
    switch (type) {
        case ContentType::HTML: {
            headers["Content-Type"] = "text/html";
            break;
        }   
        case ContentType::JSON: {
            headers["Content-Type"] = "application/json";
            break;
        }
        case ContentType::TEXT: {
            headers["Content-Type"] = "text/plain";
            break;
        }
        case ContentType::XML: {
            headers["Content-Type"] = "application/xml";
            break;
        }
        case ContentType::CSS: {
            headers["Content-Type"] = "text/css";
            break;
        }
        case ContentType::JS: {
            headers["Content-Type"] = "application/javascript";
            break;
        }
        case ContentType::JPEG: {
            headers["Content-Type"] = "image/jpeg";
            break;
        }
        case ContentType::PNG: {
            headers["Content-Type"] = "image/png";
            break;
        }
        case ContentType::GIF: {
            headers["Content-Type"] = "image/gif";
            break;
        }
        case ContentType::PDF: {
            headers["Content-Type"] = "application/pdf";
            break;
        }
        case ContentType::CSV: {
            headers["Content-Type"] = "text/csv";
            break;
        }
        case ContentType::FORM: {
            headers["Content-Type"] = "application/x-www-form-urlencoded";
            break;
        }
        default: {
            headers["Content-Type"] = "text/plain"; // Default case
            break;
        }
    }
}

void HttpResponse::set_status(int status_code) {
    switch (status_code) {
        case 200: {
            status_message = "OK";
            break;
        }
        case 201: {
            status_message = "Created";
            break;
        }
        case 204: {
            status_message = "No Content";
            break;
        }
        case 400: {
            status_message = "Bad Request";
            break;
        }
        case 401: {
            status_message = "Unauthorized";
            break;
        }
        case 403: {
            status_message = "Forbidden";
            break;
        }
        case 404: {
            status_message = "Not Found";
            break;
        }
        case 405: {
            status_message = "Method Not Allowed";
            break;
        }
        case 500: {
            status_message = "Internal Server Error";
            break;
        }
        case 502: {
            status_message = "Bad Gateway";
            break;
        }
        case 503: {
            status_message = "Service Unavailable";
            break;
        }
        case 504: {
            status_message = "Gateway Timeout";
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
    auto response = resp.to_string();
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
        } else if (bytes_sent == 0) {
            debug::log_warn("Peer closed connection");
        } else if (bytes_sent < 0) { // <= because bytes_sent == 0 is kind of weird and possibly wrong but idfc
            debug::log_error("Sending failed");
            throw std::runtime_error("Sending failed");
        }
        write_total_size += bytes_sent;
    }
}

