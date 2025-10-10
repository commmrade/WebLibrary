#include "weblib/server/Cookie.hpp"
#include <format>
#include <stdexcept>
#include "weblib/consts.hpp"
#include "weblib/exceptions.hpp"
namespace weblib {
auto Cookie::to_string() const -> std::string
{
    if (m_name.empty() || m_value.empty())
    {
        throw empty_cookie{};
    }
    std::string cookie = std::format("{}={}", m_name, m_value);
    if (!m_response_cookie)
    {
        return cookie;
    }

    cookie += "; ";
    if (m_max_age)
    {
        cookie += std::format("{}={}; ", CookieConsts::MAX_AGE, std::to_string(m_max_age.value()));
    }
    if (m_secure)
    {
        cookie += std::format("{}; ", CookieConsts::SECURE);
    }
    if (m_httpOnly)
    {
        cookie += std::format("{}; ", CookieConsts::HTTP_ONLY);
    }
    if (!m_path.empty())
    {
        cookie += std::format("{}={}; ", CookieConsts::PATH, m_path);
    }
    if (!m_domain.empty())
    {
        cookie += std::format("{}={}; ", CookieConsts::DOMAIN, m_domain);
    }
    {
        switch (m_samesite)
        {
        case SameSite::Lax:
        {
            cookie += std::format("{}={}", CookieConsts::SAME_SITE, CookieConsts::SAME_SITE_LAX);
            break;
        }
        case SameSite::Strict:
        {
            cookie += std::format("{}={}", CookieConsts::SAME_SITE, CookieConsts::SAME_SITE_STRICT);
            break;
        }
        default:
        {
            cookie += std::format("{}={}", CookieConsts::SAME_SITE, CookieConsts::SAME_SITE_NONE);
            break;
        }
        }
        cookie += "; ";
    }
    
    if (auto col_pos = cookie.find_last_of(';'); col_pos >= cookie.size() - 2)
    { // If ended with ; truncate it
       cookie.erase(col_pos);
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
} // namespace weblib