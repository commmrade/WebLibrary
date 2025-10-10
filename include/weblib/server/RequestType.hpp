// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include <cstdint>
#include <string_view>
#include <string>
#include "weblib/consts.hpp"
namespace weblib {
enum class RequestType : std::uint8_t
{
    POST,
    GET,
    PUT,
    DELETE,
    OPTIONS,
};

#define s(x) std::string{x}
inline auto req_type_to_str(RequestType req) -> std::string 
{
    if (req == RequestType::GET)
    {
        return s(HttpConsts::METHOD_GET);
    }
    else if (req == RequestType::POST)
    {
        return s(HttpConsts::METHOD_POST);
    }
    else if (req == RequestType::OPTIONS)
    {
        return s(HttpConsts::METHOD_OPTIONS);
    }
    else if (req == RequestType::PUT)
    {
        return s(HttpConsts::METHOD_PUT);
    }
    else if (req == RequestType::DELETE)
    {
        return s(HttpConsts::METHOD_DELETE);
    }
    else
    {
        return s(HttpConsts::METHOD_GET);
    }
}

inline auto req_type_from_str(std::string_view str) -> RequestType
{
    if (str == HttpConsts::METHOD_GET)
    {
        return RequestType::GET;
    }
    else if (str == HttpConsts::METHOD_POST)
    {
        return RequestType::POST;
    }
    else if (str == HttpConsts::METHOD_OPTIONS)
    {
        return RequestType::OPTIONS;
    }
    else if (str == HttpConsts::METHOD_PUT)
    {
        return RequestType::PUT;
    }
    else if (str == HttpConsts::METHOD_DELETE)
    {
        return RequestType::DELETE;
    }
    else
    {
        return RequestType::GET;
    }
}
} // namespace weblib}