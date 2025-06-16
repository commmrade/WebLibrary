#pragma once
#include <cstdint>
#include <string_view>

enum class RequestType : std::uint8_t {
    POST,
    GET,
    PUT,
    DELETE,
    OPTIONS,
};

inline auto req_type_from_str(std::string_view str) -> RequestType {
    if (str == "GET") {
        return RequestType::GET;
    } else if (str == "POST") {
        return RequestType::POST;
    } else if (str == "OPTIONS") {
        return RequestType::OPTIONS;
    } else if (str == "PUT") {
        return RequestType::PUT;
    } else if (str == "DELETE") {
        return RequestType::DELETE;
    } else {
        return RequestType::GET;
    }
}