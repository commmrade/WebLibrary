#include <server/Cookie.hpp>
#include <stdexcept>


std::string Cookie::to_string() const {
    if (name.empty() || value.empty()) {
        throw std::runtime_error("Cookie is not properly set up");
    }
    std::string final_str = name + "=" + value;
    if (!response_cookie) {
        return name + "=" + value;
    } 

    final_str += "; ";
    if (max_age) {
        final_str += "Max-Age=" + std::to_string(max_age.value()) + "; ";
    }
    if (secure) {
        final_str += "Secure; ";
    }
    if (httpOnly) {
        final_str += "HttpOnly; ";
    }
    if (!path.empty()) {
        final_str += "Path=" + path + "; ";
    }
    if (!domain.empty()) {
        final_str += "Domain=" + domain + "; ";
    }
    {
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
                final_str += "SameSite=" + "None"s;
                break;
            }
        }  
        final_str += "; "; 
    }
    
    if (final_str.find_last_of(";") >= final_str.size() - 2) { // If there is ";" at the end, truncate "; "
        final_str.resize(final_str.size() - 2);
    }
    return final_str;
}


void Cookie::set_name(std::string_view new_name) {
    name = new_name;
} 
void Cookie::set_value(std::string_view new_val) {
    value = new_val;
}

void Cookie::set_httponly(bool val) {
    httpOnly = val;
    response_cookie = true;
}
void Cookie::set_secure(bool val) {
    secure = val;
    response_cookie = true;
}
void Cookie::set_max_age(int new_max_age) {
    max_age = new_max_age;
    response_cookie = true;
}
void Cookie::set_path(std::string_view new_path) {
    path = new_path;
    response_cookie = true;
}
void Cookie::set_domain(std::string_view new_domain) {
    domain = new_domain;
    response_cookie = true;
}
void Cookie::set_samesite(SameSite new_rule) {
    response_cookie = true;
    samesite = new_rule;
}