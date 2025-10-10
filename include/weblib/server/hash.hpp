// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy

#pragma once
#include <cstdint>
#include <string>
#include <utility>

namespace weblib {
enum class RequestType : std::uint8_t;

template <class T>
void hash_combine(std::size_t &s, const T &v)
{
    std::hash<T> h;
    s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2); // I am not even gonna try to explain it
}
}

namespace std
{
template <>
struct hash<std::pair<std::string, weblib::RequestType>>
{
    size_t operator()(const std::pair<std::string, weblib::RequestType> &x) const noexcept
    {
        size_t res = 0;
        weblib::hash_combine(res, x.first);
        weblib::hash_combine(res, x.second);

        return res;
    }
};
template <>
struct hash<weblib::RequestType>
{
    size_t operator()(const weblib::RequestType &x) const noexcept
    {
        auto hash = std::hash<int>()(static_cast<int>(x));

        return hash;
    }
};
} // namespace st