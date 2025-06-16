// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy

#pragma once
#include <optional>
#include <unordered_map>
#include <string>



template <typename Map>
class InsideView {
private:
    const Map& m_headers;
public:
    explicit InsideView(const Map& m_headers) : m_headers(m_headers) {}
    using iter_type = typename Map::const_iterator;

    [[nodiscard]]
    auto get(const std::string& key) const -> std::optional<typename iter_type::value_type> {
        auto pos = m_headers.find(key);
        if (pos == m_headers.end()) {
            return std::nullopt;
        }
        return *pos;
    }

    [[nodiscard]]
    auto begin() -> iter_type {
        return iter_type{m_headers.cbegin()};
    }
    [[nodiscard]]
    auto end() -> iter_type {
        return iter_type{m_headers.cend()};
    }
    auto begin() const -> iter_type {
        return iter_type{m_headers.cbegin()};
    }
    auto end() const -> iter_type {
        return iter_type{m_headers.cend()};
    }

    [[nodiscard]]
    auto cbegin() const -> iter_type {
        return iter_type{m_headers.cbegin()};
    }
    [[nodiscard]]
    auto cend() const -> iter_type {
        return iter_type{m_headers.cend()};
    }
};
class Cookie;
using HeaderView = InsideView<std::unordered_map<std::string, std::string>>;
using CookieView = InsideView<std::unordered_map<std::string, Cookie>>;
using QueryView = HeaderView;