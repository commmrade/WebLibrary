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
    Cookie(const std::string &key, const std::string &val) : name(key), value(val) {}


    [[nodiscard]]
    std::string get_string() const {
        if (!is_resp_cookie) {
            return name + "=" + value;
        } 

        std::string final_str{};

        final_str += name + "=" + value;

        final_str += "; ";
        if (max_age) {
            final_str += "Max-Age=" + std::to_string(max_age.value());
            final_str += "; ";
        }
        if (secure) {
            final_str += "Secure";
            final_str += "; ";
        }
        if (httpOnly) {
            final_str += "HttpOnly";
            final_str += "; ";
        }
        if (!path.empty()) {
            final_str += "Path=" + path;
            final_str += "; ";
        }
        if (!domain.empty()) {
            final_str += "Domain=" + domain;
            final_str += "; ";
        }
        if (samesite != SameSite::None) {
            using std::string_literals::operator""s;
            switch (samesite) {
                case SameSite::Lax: {
                    final_str += "SameSite=" + "Lax"s;
                    break;
                }
                case SameSite::Strict: {
                    final_str += "SameSite=" + "Strict"s;
                    break;
                }
                default: {
                    break;
                }
            }
            final_str += "; ";
        }
      
        if (final_str.find_last_of(";") == final_str.size() - 2) {
            final_str.erase(final_str.find_last_of(";"));
        }

        return final_str;
    }

    [[nodiscard]]
    std::string get_name() const {
        return name;
    }

    [[nodiscard]]
    std::string get_value() const {
        return value;
    }
    [[nodiscard]]
    std::optional<int> get_max_age() const {
        return max_age;
    }
    [[nodiscard]]
    std::optional<bool> get_httponly() const {
        return httpOnly;
    }
    [[nodiscard]]
    std::optional<bool> get_is_secure() const {
        return secure;
    }
    [[nodiscard]]
    SameSite get_samesite_policy() const {
        return samesite;
    }

    void set_name(const std::string &new_name) {
        name = new_name;
    } 
    void set_val(const std::string &new_val) {
        value = new_val;
    }

    void set_httponly(bool val) {
        httpOnly = val;
        is_resp_cookie = true;
    }
    void set_secure(bool val) {
        secure = val;
        is_resp_cookie = true;
    }
    void set_max_age(int new_max_age) {
        max_age = new_max_age;
        is_resp_cookie = true;
    }
    void set_path(const std::string &new_path) {
        path = new_path;
        is_resp_cookie = true;
    }
    void set_domain(const std::string &new_domain) {
        domain = new_domain;
        is_resp_cookie = true;
    }
    void set_samesite(SameSite new_rule) {
        samesite = new_rule;
    }



private:
    bool httpOnly{false};
    bool secure{false};
    std::optional<int> max_age;
    std::string path;
    std::string domain;
    SameSite samesite{SameSite::None};

    std::string name;
    std::string value;

    bool is_resp_cookie{false};
};