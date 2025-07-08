// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy

#pragma once

#include <cstdint>
#include <optional>
#include <string>


enum class SameSite : std::uint8_t {
    None,
    Lax,
    Strict,
};

class Cookie {
public:
    explicit Cookie(std::string_view key, std::string_view val) : m_name(key), m_value(val) {}

    Cookie() = default;

    [[nodiscard]]
    auto to_string() const -> std::string;

    [[nodiscard]]
    auto get_name() const -> std::string {
        return m_name;
    }
    [[nodiscard]]
    auto get_value() const -> std::string {
        return m_value;
    }
    [[nodiscard]]
    auto get_max_age() const -> std::optional<int> {
        return std::optional<int>{m_max_age};
    }
    [[nodiscard]]
    auto get_httponly() const -> std::optional<bool> {
        return std::optional<bool>{m_httpOnly};
    }
    [[nodiscard]]
    auto get_is_secure() const -> std::optional<bool> {
        return std::optional<bool>{m_secure};
    }
    [[nodiscard]]
    auto get_samesite_policy() const -> SameSite {
        return m_samesite;
    }


    void set_name(std::string_view name);
    void set_value(std::string_view new_val);

    void set_httponly(bool val);
    void set_secure(bool val);
    void set_max_age(int new_max_age);
    void set_path(std::string_view new_path);
    void set_domain(std::string_view new_domain);
    void set_samesite(SameSite new_rule);



private:
    bool m_httpOnly{false};
    bool m_secure{false};
    std::optional<int> m_max_age;
    std::string m_path;
    std::string m_domain;
    SameSite m_samesite{SameSite::None};

    std::string m_name;
    std::string m_value;

    bool m_response_cookie{false};
};

class CookieBuilder {
private:
    Cookie m_cookie;
public:
    auto set_name(std::string_view name) -> CookieBuilder& {
        m_cookie.set_name(name);
        return *this;
    }
    auto set_value(std::string_view new_val) -> CookieBuilder& {
        m_cookie.set_value(new_val);
        return *this;
    }

    auto set_httponly(bool val) -> CookieBuilder& {
        m_cookie.set_httponly(val);
        return *this;
    }
    auto set_secure(bool val) -> CookieBuilder& {
        m_cookie.set_secure(val);
        return *this;
    }
    auto set_max_age(int new_max_age) -> CookieBuilder& {
        m_cookie.set_max_age(new_max_age);
        return *this;
    }
    auto set_path(std::string_view new_path) -> CookieBuilder& {
        m_cookie.set_path(new_path);
        return *this;
    }
    auto set_domain(std::string_view new_domain) -> CookieBuilder& {
        m_cookie.set_domain(new_domain);
        return *this;
    }
    auto set_samesite(SameSite new_rule) -> CookieBuilder& {
        m_cookie.set_samesite(new_rule);
        return *this;
    }

    auto build() -> Cookie {
        return std::move(m_cookie);
    }

};