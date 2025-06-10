#include <server/Cookie.hpp>
#include <stdexcept>


std::string Cookie::to_string() const {
    if (m_name.empty() || m_value.empty()) {
        throw std::runtime_error("Cookie is not properly set up");
    }
    std::string final_str = m_name + "=" + m_value;
    if (!m_response_cookie) {
        return m_name + "=" + m_value;
    } 

    final_str += "; ";
    if (m_max_age) {
        final_str += "Max-Age=" + std::to_string(m_max_age.value()) + "; ";
    }
    if (m_secure) {
        final_str += "Secure; ";
    }
    if (m_httpOnly) {
        final_str += "HttpOnly; ";
    }
    if (!m_path.empty()) {
        final_str += "Path=" + m_path + "; ";
    }
    if (!m_domain.empty()) {
        final_str += "Domain=" + m_domain + "; ";
    }
    {
        using std::string_literals::operator""s;
        switch (m_samesite) {
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
    m_name = new_name;
} 
void Cookie::set_value(std::string_view new_val) {
    m_value = new_val;
}

void Cookie::set_httponly(bool val) {
    m_httpOnly = val;
    m_response_cookie = true;
}
void Cookie::set_secure(bool val) {
    m_secure = val;
    m_response_cookie = true;
}
void Cookie::set_max_age(int new_max_age) {
    m_max_age = new_max_age;
    m_response_cookie = true;
}
void Cookie::set_path(std::string_view new_path) {
    m_path = new_path;
    m_response_cookie = true;
}
void Cookie::set_domain(std::string_view new_domain) {
    m_domain = new_domain;
    m_response_cookie = true;
}
void Cookie::set_samesite(SameSite new_rule) {
    m_response_cookie = true;
    m_samesite = new_rule;
}