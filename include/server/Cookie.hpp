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
    std::string get_string() const;

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


    void set_name(const std::string &new_name);
    void set_val(const std::string &new_val);

    void set_httponly(bool val);
    void set_secure(bool val);
    void set_max_age(int new_max_age);
    void set_path(const std::string &new_path);
    void set_domain(const std::string &new_domain);
    void set_samesite(SameSite new_rule);



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