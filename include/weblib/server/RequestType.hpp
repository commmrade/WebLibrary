// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include <cstdint>
#include <string_view>
#include <string>

enum class RequestType : std::uint8_t
{
    POST,
    GET,
    PUT,
    DELETE,
    OPTIONS,
};

inline auto req_type_to_str(RequestType req) -> std::string 
{
    if (req == RequestType::GET)
    {
        return "GET";
    }
    else if (req == RequestType::POST)
    {
        return "POST";
    }
    else if (req == RequestType::OPTIONS)
    {
        return "OPTIONS";
    }
    else if (req == RequestType::PUT)
    {
        return "PUT";
    }
    else if (req == RequestType::DELETE)
    {
        return "DELETE";
    }
    else
    {
        return "GET";
    }
}

inline auto req_type_from_str(std::string_view str) -> RequestType
{
    if (str == "GET")
    {
        return RequestType::GET;
    }
    else if (str == "POST")
    {
        return RequestType::POST;
    }
    else if (str == "OPTIONS")
    {
        return RequestType::OPTIONS;
    }
    else if (str == "PUT")
    {
        return RequestType::PUT;
    }
    else if (str == "DELETE")
    {
        return RequestType::DELETE;
    }
    else
    {
        return RequestType::GET;
    }
}