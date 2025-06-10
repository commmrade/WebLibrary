#pragma once

#include <optional>
#include <string>


enum class SameSite {
    None,
    Lax,
    Strict,
};

class Cookie {
public:
    explicit Cookie(std::string_view key, std::string_view val) : m_name(key), m_value(val) {}

    Cookie() = default;

    [[nodiscard]]
    std::string to_string() const;

    [[nodiscard]]
    std::string get_name() const {
        return m_name;
    }
    [[nodiscard]]
    std::string get_value() const {
        return m_value;
    }
    [[nodiscard]]
    std::optional<int> get_max_age() const {
        return m_max_age;
    }
    [[nodiscard]]
    std::optional<bool> get_httponly() const {
        return m_httpOnly;
    }
    [[nodiscard]]
    std::optional<bool> get_is_secure() const {
        return m_secure;
    }
    [[nodiscard]]
    SameSite get_samesite_policy() const {
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
    CookieBuilder& set_name(std::string_view name) {
        m_cookie.set_name(name);
        return *this;
    }
    CookieBuilder& set_value(std::string_view new_val) {
        m_cookie.set_value(new_val);
        return *this;
    }

    CookieBuilder& set_httponly(bool val) {
        m_cookie.set_httponly(val);
        return *this;
    }
    CookieBuilder& set_secure(bool val) {
        m_cookie.set_secure(val);
        return *this;
    }
    CookieBuilder& set_max_age(int new_max_age) {
        m_cookie.set_max_age(new_max_age);
        return *this;
    }
    CookieBuilder& set_path(std::string_view new_path) {
        m_cookie.set_path(new_path);
        return *this;
    }
    CookieBuilder& set_domain(std::string_view new_domain) {
        m_cookie.set_domain(new_domain);
        return *this;
    }
    CookieBuilder& set_samesite(SameSite new_rule) {
        m_cookie.set_samesite(new_rule);
        return *this;
    }

    Cookie build() {
        return std::move(m_cookie);
    }

};