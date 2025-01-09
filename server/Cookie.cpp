#include <server/Cookie.hpp>


std::string Cookie::get_string() const {
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


void Cookie::set_name(std::string_view new_name) {
    name = new_name;
} 
void Cookie::set_val(std::string_view new_val) {
    value = new_val;
}

void Cookie::set_httponly(bool val) {
    httpOnly = val;
    is_resp_cookie = true;
}
void Cookie::set_secure(bool val) {
    secure = val;
    is_resp_cookie = true;
}
void Cookie::set_max_age(int new_max_age) {
    max_age = new_max_age;
    is_resp_cookie = true;
}
void Cookie::set_path(std::string_view new_path) {
    path = new_path;
    is_resp_cookie = true;
}
void Cookie::set_domain(std::string_view new_domain) {
    domain = new_domain;
    is_resp_cookie = true;
}
void Cookie::set_samesite(SameSite new_rule) {
    samesite = new_rule;
}