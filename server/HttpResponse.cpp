#include "weblib/server/HttpResponse.hpp"
#include "weblib/debug.hpp"
#include <cerrno>
#include <format>
#include <print>
#include <stdexcept>
#include <string_view>
#include <sys/poll.h>
#include "weblib/consts.hpp"
#include "weblib/exceptions.hpp"

auto HttpResponse::to_string() const -> std::string
{
    std::string response =
        std::format("{}/{} {} {}\r\n", HttpConsts::HTTP, m_http_version, m_status_code, m_status_message);
    for (const auto &[header_name, header_value] : m_headers)
    {
        response += std::format("{}: {}\r\n", header_name, header_value);
    }
    if (!m_headers.contains(HeaderConsts::CONTENT_LENGTH.data()))
    {
        response += std::format("{}: {}\r\n", HeaderConsts::CONTENT_LENGTH, m_body.size());
    }
    response += HttpConsts::CRNL;
    response += m_body;
    return response;
}

void HttpResponse::set_header(const std::string &name, std::string_view value)
{
    if (name == HeaderConsts::SET_COOKIE)
    {
        throw std::runtime_error("Use add_cookie() instead!");
    }
    m_headers[name] = value;
}

void HttpResponse::set_cookie(const Cookie &cookie)
{
    std::string const cookie_str = cookie.to_string();
    m_headers[std::string{HeaderConsts::SET_COOKIE}] = cookie_str;
}

void HttpResponse::set_header(HeaderType header_type, std::string_view value)
{
    switch (header_type)
    {
    case HeaderType::CONTENT_TYPE:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = value;
        break;
    }
    case HeaderType::CONTENT_LENGTH:
    {
        m_headers[std::string{HeaderConsts::CONTENT_LENGTH}] = value;
        break;
    }
    case HeaderType::CACHE_CONTROL:
    {
        m_headers[std::string{HeaderConsts::CACHE_CONTROL}] = value;
        break;
    }
    case HeaderType::EXPIRES:
    {
        m_headers[std::string{HeaderConsts::EXPIRES}] = value;
        break;
    }
    case HeaderType::SET_COOKIE:
    {
        m_headers[std::string{HeaderConsts::SET_COOKIE}] = value;
        break;
    }
    case HeaderType::LOCATION:
    {
        m_headers[std::string{HeaderConsts::LOCATION}] = value;
        break;
    }
    case HeaderType::SERVER:
    {
        m_headers[std::string{HeaderConsts::SERVER}] = value;
        break;
    }
    case HeaderType::ACCEPT:
    {
        m_headers[std::string{HeaderConsts::ACCEPT}] = value;
        break;
    }
    case HeaderType::USER_AGENT:
    {
        m_headers[std::string{HeaderConsts::USER_AGENT}]= value;
        break;
    }
    case HeaderType::HOST:
    {
        m_headers[std::string{HeaderConsts::HOST}]= value;
        break;
    }
    case HeaderType::ACCEPT_LANGUAGE:
    {
        m_headers[std::string{HeaderConsts::ACCEPT_LANGUAGE}] = value;
        break;
    }
    default:
    {
        throw std::runtime_error("Use set_header(string)");
    }
    }
}

void HttpResponse::remove_header(const std::string &name)
{
    m_headers.erase(name); // Removing if exists (doesn't throw if does not exist)
}

void HttpResponse::set_content_type(ContentType type)
{
    switch (type)
    {
    case ContentType::HTML:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_TEXT_HTML; 
        break;
    }
    case ContentType::JSON:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_APP_JSON;
        break;
    }
    case ContentType::TEXT:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_TEXT_PLAIN; 
        break;
    }
    case ContentType::XML:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_APP_XML;
        break;
    }
    case ContentType::CSS:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_TEXT_CSS; 
        break;
    }
    case ContentType::JS:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_APP_JAVASCRIPT;
        break;
    }
    case ContentType::JPEG:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_IMAGE_JPEG;
        break;
    }
    case ContentType::PNG:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_IMAGE_PNG;
        break;
    }
    case ContentType::GIF:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_IMAGE_GIF;
        break;
    }
    case ContentType::PDF:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_APP_PDF;
        break;
    }
    case ContentType::CSV:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_TEXT_CSV;
        break;
    }
    case ContentType::FORM:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_APP_ENCODED; 
        break;
    }
    default:
    {
        m_headers[std::string{HeaderConsts::CONTENT_TYPE}] = HeaderConsts::CONTENT_TYPE_TEXT_PLAIN; // Default case
        break;
    }
    }
}

void HttpResponse::set_body_json(const Json::Value &json_obj)
{
    Json::StreamWriterBuilder w;
    std::string               s = Json::writeString(w, json_obj);
    m_body                      = std::move(s);
}

void HttpResponse::set_status(int m_status_code)
{
    switch (m_status_code)
    {
    case OK:
    {
        m_status_message = HttpConsts::STATUS_OK;
        break;
    }
    case CREATED:
    {
        m_status_message = HttpConsts::STATUS_CREATED;
        break;
    }
    case NO_CONTENT:
    {
        m_status_message = HttpConsts::STATUS_NO_CONTENT;
        break;
    }
    case BAD_REQUEST:
    {
        m_status_message = HttpConsts::STATUS_BAD_REQUEST;
        break;
    }
    case UNAUTHORIZED:
    {
        m_status_message = HttpConsts::STATUS_UNAUTHORIZED;
        break;
    }
    case FORBIDDEN:
    {
        m_status_message = HttpConsts::STATUS_FORBIDDEN;
        break;
    }
    case NOT_FOUND:
    {
        m_status_message = HttpConsts::STATUS_NOT_FOUND;
        break;
    }
    case METHOD_NOT_ALLOWED:
    {
        m_status_message = HttpConsts::STATUS_METHOD_NOT_ALLOWED;
        break;
    }
    case INTERNAL_SERVER_ERROR:
    {
        m_status_message = HttpConsts::STATUS_INTERNAL_SERVER_ERROR;
        break;
    }
    case BAD_GATEWAY:
    {
        m_status_message = HttpConsts::STATUS_BAD_GATEWAY;
        break;
    }
    case SERVICE_UNAVAILABLE:
    {
        m_status_message = HttpConsts::STATUS_SERVICE_UNAVAILABLE;
        break;
    }
    case GATEWAY_TIMEOUT:
    {
        m_status_message = HttpConsts::STATUS_GATEWAY_TIMEOUT;
        break;
    }
    default:
    {
        m_status_message = "Unknown";
        break;
    }
    }
    this->m_status_code = m_status_code;
}

void HttpResponseWriter::respond(HttpResponse &resp)
{
    auto   response         = resp.to_string();
    size_t write_total_size = 0;

    while (write_total_size < response.size())
    {
        ssize_t const bytes_sent = send(m_client_socket, response.c_str() + write_total_size,
                                        response.size() - write_total_size, 0);
        if (bytes_sent < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
        {
            pollfd client{};
            client.fd     = m_client_socket;
            client.events = POLLOUT;

            int const poll_result = poll(&client, 1, MAX_WAIT);
            if (poll_result <= 0)
            {
                debug::log_error("Connection is lost");
                return;
            }
            continue;
        }
        if (bytes_sent == 0)
        {
            debug::log_warn("Peer closed connection");
        }
        else if (bytes_sent < 0)
        { // <= because bytes_sent == 0 is kind of weird and possibly wrong but idfc
            debug::log_error("Sending failed");
            throw writing_socket_error{};
        }
        write_total_size += bytes_sent;
    }
}
