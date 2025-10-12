#include "weblib/server/Cookie.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

using namespace weblib;

TEST(CookieRequestTest, NoTrailingSemicolon)
{
    Cookie cookie;
    cookie.set_name("Session-id");
    cookie.set_value("23472347234723");
    ASSERT_EQ(cookie.to_string(), "Session-id=23472347234723");
}

TEST(CookieRequestTest, ThrowsOnEmptyToString)
{
    Cookie cookie;
    cookie.set_value("cock");
    std::string a;
    ASSERT_THROW((a = cookie.to_string()), std::runtime_error);
}

TEST(CookieResponseTest, NoTrailingSemicolon)
{
    {
        Cookie cookie;
        cookie.set_name("Session-id");
        cookie.set_value("23472347234723");
        cookie.set_httponly(true);
        cookie.set_path("/cock");
        ASSERT_EQ(cookie.to_string(),
                  "Session-id=23472347234723; HttpOnly; Path=/cock; SameSite=None");
    }
    {
        Cookie cookie;
        cookie.set_name("Session-id");
        cookie.set_value("23472347234723");
        cookie.set_samesite(SameSite::Lax);
        cookie.set_secure(true);
        ASSERT_EQ(cookie.to_string(), "Session-id=23472347234723; Secure; SameSite=Lax");
    }
    {
        Cookie cookie;
        cookie.set_name("Session-id");
        cookie.set_value("23472347234723");
        ASSERT_EQ(cookie.to_string(), "Session-id=23472347234723");
    }
    {
        Cookie cookie;
        cookie.set_name("Session-id");
        std::string a;
        ASSERT_THROW((a = cookie.to_string()), std::runtime_error);
    }
}
