#pragma once
#include <cstdint>
enum class HeaderType : std::uint8_t {
    CONTENT_TYPE,        // Indicates the media type of the resource
    CONTENT_LENGTH,      // Specifies the size of the response body in bytes
    CACHE_CONTROL,       // Directives for caching mechanisms
    EXPIRES,             // Indicates when the response is considered stale
    SET_COOKIE,          // Sends cookies from the server to the client
    LOCATION,            // URL to redirect to
    SERVER,              // Information about the server software
    ACCEPT,              // Specifies the media types acceptable for the response
    USER_AGENT,         // Information about the client software
    HOST,                // Domain name of the server
    ACCEPT_LANGUAGE      // Preferred languages for the response
};

enum class ContentType : std::uint8_t {
    HTML,
    JSON,
    TEXT,
    XML,
    CSS,
    JS,
    JPEG,
    PNG,
    GIF,
    PDF,
    CSV,
    FORM,
};