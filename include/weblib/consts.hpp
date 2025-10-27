#pragma once
#include <string_view>
namespace weblib
{
struct HttpConsts
{
    static constexpr std::string_view HTTP     = "HTTP";
    static constexpr std::string_view HTTP_VERSION = "1.1";
    static constexpr std::string_view CRNL     = "\r\n";
    static constexpr std::string_view CRNLCRNL = "\r\n\r\n";

    static constexpr std::string_view METHOD_GET     = "GET";
    static constexpr std::string_view METHOD_POST    = "POST";
    static constexpr std::string_view METHOD_OPTIONS = "OPTIONS";
    static constexpr std::string_view METHOD_PUT     = "PUT";
    static constexpr std::string_view METHOD_DELETE  = "DELETE";

    // Статусные сообщения
    static constexpr std::string_view STATUS_OK                    = "OK";
    static constexpr std::string_view STATUS_CREATED               = "Created";
    static constexpr std::string_view STATUS_NO_CONTENT            = "No Content";
    static constexpr std::string_view STATUS_BAD_REQUEST           = "Bad Request";
    static constexpr std::string_view STATUS_UNAUTHORIZED          = "Unauthorized";
    static constexpr std::string_view STATUS_FORBIDDEN             = "Forbidden";
    static constexpr std::string_view STATUS_NOT_FOUND             = "Not Found";
    static constexpr std::string_view STATUS_METHOD_NOT_ALLOWED    = "Method Not Allowed";
    static constexpr std::string_view STATUS_INTERNAL_SERVER_ERROR = "Internal Server Error";
    static constexpr std::string_view STATUS_BAD_GATEWAY           = "Bad Gateway";
    static constexpr std::string_view STATUS_SERVICE_UNAVAILABLE   = "Service Unavailable";
    static constexpr std::string_view STATUS_GATEWAY_TIMEOUT       = "Gateway Timeout";
    static constexpr std::string_view STATUS_UNKNOWN = "Unknown status code, set msg manually";
};

struct HeaderConsts
{
    static constexpr std::string_view COOKIE_HEADER  = "cookie";
    static constexpr std::string_view SET_COOKIE     = "Set-Cookie";
    static constexpr std::string_view CONTENT_LENGTH = "Content-Length";

    static constexpr std::string_view CONTENT_TYPE                = "Content-Type";
    static constexpr std::string_view CONTENT_TYPE_TEXT_HTML      = "text/html";
    static constexpr std::string_view CONTENT_TYPE_TEXT_PLAIN     = "text/plain";
    static constexpr std::string_view CONTENT_TYPE_TEXT_CSS       = "text/css";
    static constexpr std::string_view CONTENT_TYPE_TEXT_CSV       = "text/csv";
    static constexpr std::string_view CONTENT_TYPE_APP_JSON       = "application/json";
    static constexpr std::string_view CONTENT_TYPE_APP_XML        = "application/xml";
    static constexpr std::string_view CONTENT_TYPE_APP_JAVASCRIPT = "application/javascript";
    static constexpr std::string_view CONTENT_TYPE_APP_ENCODED =
        "application/x-www-form-urlencoded";
    static constexpr std::string_view CONTENT_TYPE_APP_PDF    = "application/pdf";
    static constexpr std::string_view CONTENT_TYPE_IMAGE_GIF  = "image/gif";
    static constexpr std::string_view CONTENT_TYPE_IMAGE_JPEG = "image/jpeg";
    static constexpr std::string_view CONTENT_TYPE_IMAGE_PNG  = "image/png";

    static constexpr std::string_view CACHE_CONTROL   = "Cache-Control";
    static constexpr std::string_view EXPIRES         = "Expires";
    static constexpr std::string_view LOCATION        = "Location";
    static constexpr std::string_view SERVER          = "Server";
    static constexpr std::string_view ACCEPT          = "Accept";
    static constexpr std::string_view USER_AGENT      = "User-Agent";
    static constexpr std::string_view HOST            = "Host";
    static constexpr std::string_view ACCEPT_LANGUAGE = "Accept-Language";
};

struct CookieConsts
{
    static constexpr std::string_view MAX_AGE   = "Max-Age";
    static constexpr std::string_view SECURE    = "Secure";
    static constexpr std::string_view HTTP_ONLY = "HttpOnly";
    static constexpr std::string_view PATH      = "Path";
    static constexpr std::string_view DOMAIN    = "Domain";

    static constexpr std::string_view SAME_SITE        = "SameSite";
    static constexpr std::string_view SAME_SITE_LAX    = "Lax";
    static constexpr std::string_view SAME_SITE_STRICT = "Strict";
    static constexpr std::string_view SAME_SITE_NONE   = "None";
};
} // namespace weblib