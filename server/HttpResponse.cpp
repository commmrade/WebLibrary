#include "weblib/server/HttpResponse.hpp"
#include "debug.hpp"
#include <cerrno>
#include <format>
#include <print>
#include <stdexcept>
#include <string_view>
#include <sys/poll.h>


auto HttpResponse::to_string() const -> std::string {
    std::string response = std::format("HTTP/{} {} {}\r\n", m_http_version, m_status_code, m_status_message);
    // Adding m_headers part
    for (const auto &[header_name, header_value] : m_headers) {
        response += std::format("{}: {}\r\n", header_name, header_value);
    }
    if (!m_headers.contains("Content-Length")) { // Setting Content-Length if not already set
        response += std::format("Content-Length: {}\r\n", m_body.size());
    }
    response += "\r\n"; // Separating m_headers and answer par
    response += m_body; // Adding user text
    return response;
}


void HttpResponse::set_header_raw(const std::string& name, std::string_view value) {
    if (name == "Set-Cookie") {
        throw std::runtime_error("Use add_cookie() instead!");
    }
    m_headers[name] = value;
}

void HttpResponse::set_cookie(const Cookie &cookie) {
    std::string const cookie_str = cookie.to_string();
    m_headers["Set-Cookie"] = cookie_str;
}

void HttpResponse::set_header(HeaderType header_type, std::string_view value) {
    switch (header_type) {
        case HeaderType::CONTENT_TYPE: {
            m_headers["Content-Type"] = value;
            break;
        }
        case HeaderType::CONTENT_LENGTH: {
            m_headers["Content-Length"] = value;
            break;
        }
        case HeaderType::CACHE_CONTROL: {
            m_headers["Cache-Control"] = value;
            break;
        }
        case HeaderType::EXPIRES: {
            m_headers["Expires"] = value;
            break;
        }
        case HeaderType::SET_COOKIE: {
            m_headers["Set-Cookie"] = value;
            break;
        }
        case HeaderType::LOCATION: {
            m_headers["Location"] = value;
            break;
        }
        case HeaderType::SERVER: {
            m_headers["Server"] = value;
            break;
        }
        case HeaderType::ACCEPT: {
            m_headers["Accept"] = value;
            break;
        }
        case HeaderType::USER_AGENT: {
            m_headers["User-Agent"] = value;
            break;
        }
        case HeaderType::HOST: {
            m_headers["Host"] = value;
            break;
        }
        case HeaderType::ACCEPT_LANGUAGE: {
            m_headers["Accept-Language"] = value;
            break;
        }
        default: {
            throw std::runtime_error("Header type is not implemented > todo!");
        }
    }
}

void HttpResponse::remove_header(const std::string& name) {
    m_headers.erase(name); // Removing if exists (doesn't throw if does not exist)
}  

void HttpResponse::set_content_type(ContentType type) {
    switch (type) {
        case ContentType::HTML: {
            m_headers["Content-Type"] = "text/html";
            break;
        }   
        case ContentType::JSON: {
            m_headers["Content-Type"] = "application/json";
            break;
        }
        case ContentType::TEXT: {
            m_headers["Content-Type"] = "text/plain";
            break;
        }
        case ContentType::XML: {
            m_headers["Content-Type"] = "application/xml";
            break;
        }
        case ContentType::CSS: {
            m_headers["Content-Type"] = "text/css";
            break;
        }
        case ContentType::JS: {
            m_headers["Content-Type"] = "application/javascript";
            break;
        }
        case ContentType::JPEG: {
            m_headers["Content-Type"] = "image/jpeg";
            break;
        }
        case ContentType::PNG: {
            m_headers["Content-Type"] = "image/png";
            break;
        }
        case ContentType::GIF: {
            m_headers["Content-Type"] = "image/gif";
            break;
        }
        case ContentType::PDF: {
            m_headers["Content-Type"] = "application/pdf";
            break;
        }
        case ContentType::CSV: {
            m_headers["Content-Type"] = "text/csv";
            break;
        }
        case ContentType::FORM: {
            m_headers["Content-Type"] = "application/x-www-form-urlencoded";
            break;
        }
        default: {
            m_headers["Content-Type"] = "text/plain"; // Default case
            break;
        }
    }
}

void HttpResponse::set_status(int m_status_code) {
    switch (m_status_code) {
        case OK: {
            m_status_message = "OK";
            break;
        }
        case CREATED: {
            m_status_message = "Created";
            break;
        }
        case NO_CONTENT: {
            m_status_message = "No Content";
            break;
        }
        case BAD_REQUEST: {
            m_status_message = "Bad Request";
            break;
        }
        case UNAUTHORIZED: {
            m_status_message = "Unauthorized";
            break;
        }
        case FORBIDDEN: {
            m_status_message = "Forbidden";
            break;
        }
        case NOT_FOUND: {
            m_status_message = "Not Found";
            break;
        }
        case METHOD_NOT_ALLOWED: {
            m_status_message = "Method Not Allowed";
            break;
        }
        case INTERNAL_SERVER_ERROR: {
            m_status_message = "Internal Server Error";
            break;
        }
        case BAD_GATEWAY: {
            m_status_message = "Bad Gateway";
            break;
        }
        case SERVICE_UNAVAILABLE: {
            m_status_message = "Service Unavailable";
            break;
        }
        case GATEWAY_TIMEOUT: {
            m_status_message = "Gateway Timeout";
            break;
        }
        default: {
            m_status_message = "Unknown status code, set msg manually";
            break;
        }
    }
    this->m_status_code = m_status_code;
}


void HttpResponseWriter::respond(HttpResponse &resp) { // Sending response text to the requester
    auto response = resp.to_string();
    size_t write_total_size = 0;

    while (write_total_size < response.size()) {
        ssize_t const bytes_sent = send(m_client_socket, response.c_str() + write_total_size, response.size() - write_total_size, 0);
        if (bytes_sent < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            pollfd client{};
            client.fd = m_client_socket;
            client.events = POLLOUT;

            int const poll_result = poll(&client, 1, MAX_WAIT);
            if (poll_result <= 0) {
                debug::log_error("Connection is lost");
                return;
            }
            continue;
        }
        if (bytes_sent == 0) {
            debug::log_warn("Peer closed connection");
        } else if (bytes_sent < 0) { // <= because bytes_sent == 0 is kind of weird and possibly wrong but idfc
            debug::log_error("Sending failed");
            throw std::runtime_error("Sending failed");
        }
        write_total_size += bytes_sent;
    }
}

