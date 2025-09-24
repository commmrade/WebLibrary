#pragma once
// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#include <cstdint>
#include <stdexcept>
#include <string>

class HttpRequest;
class HttpQuery;

class Query {
private:
    std::string m_content{};
    friend HttpRequest;
    friend HttpQuery;

    Query() = default;    
public:
    [[nodiscard]]
    auto as_str() -> std::string {
        return m_content;
    }

    template <typename T>
    [[nodiscard]]
    auto as() const -> T {
        return m_content;
    } 

};

template <>
[[nodiscard]]
inline auto Query::as<float>() const -> float {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stof(m_content);
    return converted_val;
}
template <>
[[nodiscard]]
inline auto Query::as<int>() const -> int {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stoi(m_content);
    return converted_val;
}
template <>
[[nodiscard]]
inline auto Query::as<int64_t>() const -> int64_t {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stoll(m_content);
    return converted_val;
}
template <>
[[nodiscard]]
inline auto Query::as<double>() const -> double {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stod(m_content);
    return converted_val;
}
template<>
[[nodiscard]]
inline auto Query::as<const char*>() const -> const char* {
    return m_content.c_str();
}
