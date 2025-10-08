#include "weblib/server/Cookie.hpp"
#include <stdexcept>

auto Cookie::to_string() const -> std::string
{
    if (m_name.empty() || m_value.empty())
    {
        throw std::runtime_error("Cookie is not properly set up");
    }
    std::string cookie = m_name + "=" + m_value;
    if (!m_response_cookie)
    {
        return m_name + "=" + m_value;
    }

    cookie += "; ";
    if (m_max_age)
    {
        cookie += "Max-Age=" + std::to_string(m_max_age.value()) + "; ";
    }
    if (m_secure)
    {
        cookie += "Secure; ";
    }
    if (m_httpOnly)
    {
        cookie += "HttpOnly; ";
    }
    if (!m_path.empty())
    {
        cookie += "Path=" + m_path + "; ";
    }
    if (!m_domain.empty())
    {
        cookie += "Domain=" + m_domain + "; ";
    }
    {
        using std::string_literals::operator""s;
        switch (m_samesite)
        {
        case SameSite::Lax:
        {
            cookie += "SameSite=" + "Lax"s;
            break;
        }
        case SameSite::Strict:
        {
            cookie += "SameSite=" + "Strict"s;
            break;
        }
        default:
        {
            cookie += "SameSite=" + "None"s;
            break;
        }
        }
        cookie += "; ";
    }

    if (cookie.find_last_of(';') >= cookie.size() - 2)
    { // If there is ";" at the end, truncate "; "
        cookie.resize(cookie.size() - 2);
    }
    return cookie;
}

void Cookie::set_name(std::string_view new_name) { m_name = new_name; }
void Cookie::set_value(std::string_view new_val) { m_value = new_val; }

void Cookie::set_httponly(bool val)
{
    m_httpOnly        = val;
    m_response_cookie = true;
}
void Cookie::set_secure(bool val)
{
    m_secure          = val;
    m_response_cookie = true;
}
void Cookie::set_max_age(int new_max_age)
{
    m_max_age         = new_max_age;
    m_response_cookie = true;
}
void Cookie::set_path(std::string_view new_path)
{
    m_path            = new_path;
    m_response_cookie = true;
}
void Cookie::set_domain(std::string_view new_domain)
{
    m_domain          = new_domain;
    m_response_cookie = true;
}
void Cookie::set_samesite(SameSite new_rule)
{
    m_response_cookie = true;
    m_samesite        = new_rule;
}
