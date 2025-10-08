#include "weblib/server/RequestType.hpp"
#include <optional>
#include <ostream>
#include <weblib/server/HttpRequest.hpp>
#include <gtest/gtest.h>
#include <vector>

#include <print>
TEST(HttpRequestParsing, GetRequestQueries)
{
    std::string request_str =
        "GET /dashboard/42?page=1&moron=55 HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Cookie: session_id=abc123; theme=dark\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n";

    std::vector<std::string> param_names{"user", "page", "moron"};
    HttpRequest request{request_str, "/dashboard/{user}?page={page}&moron={moron}", param_names};

    ASSERT_EQ(request.get_query("user").as<int>(), 42);
    ASSERT_EQ(request.get_query("page").as_str(), "1");
    ASSERT_EQ(request.get_query("moron").as<int>(), 55);
    ASSERT_EQ(request.get_query("cockfuck").as_str(), "");
}

// TEST(HttpRequestParsing, GetRequestQueries0) {
//     std::string request_str =
//     "GET /42 HTTP/1.1\r\n"
//     "Host: example.com\r\n"
//     "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko)
//     Chrome/123.0.0.0 Safari/537.36\r\n" "Accept:
//     text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
//     "Accept-Language: en-US,en;q=0.9\r\n"
//     "Accept-Encoding: gzip, deflate, br\r\n"
//     "Connection: keep-alive\r\n"
//     "Cookie: session_id=abc123; theme=dark\r\n"
//     "Upgrade-Insecure-Requests: 1\r\n"
//     "\r\n";

//     std::vector<std::string> param_names{"user"};
//     HttpRequest request{false, request_str, "/{user}", param_names};

//     ASSERT_EQ(request.get_query("user").as<int>(), 42);
// }

TEST(HttpRequestParsing, GetRequestQueries2)
{
    std::string request_str =
        "GET /auth/cock/fuck?age=42 HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Cookie: session_id=abc123; theme=dark\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n";

    std::vector<std::string> param_names{"age"};
    HttpRequest              request{request_str, "/auth/cock/fuck?age={age}", param_names};

    ASSERT_EQ(request.get_query("age").as<int>(), 42);
}

TEST(HttpRequestParsing, GetRequestQueries3)
{
    std::string request_str =
        "GET /auth/233/fuck?age=42 HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Cookie: session_id=abc123; theme=dark\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n";

    std::vector<std::string> param_names{"id", "age"};
    HttpRequest              request{request_str, "/auth/{id}/fuck?age={age}", param_names};
    ASSERT_EQ(request.get_query("id").as<int>(), 233);
    ASSERT_EQ(request.get_query("age").as<int>(), 42);
}

TEST(HttpRequestParsing, CookiesTest)
{
    std::string request_str =
        "GET /dashboard?user=42&page=1&moron=55 HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Cookie: session_id=abc123; theme=dark\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n";
    std::vector<std::string> param_names{"user", "page", "moron"};
    HttpRequest request{request_str, "/dashboard?user={user}&page={page}&moron={moron}",
                        param_names};
    auto        view = request.get_cookies();
    for (const auto &cookies : view)
    {
        std::println("'{}' '{}'", cookies.second.get_value(), cookies.second.get_name().c_str());
    }
    // ASSERT_EQ(request.get_cookie("session_id")->get_value(), "");
}

TEST(HttpRequestParsing, GetRequestHeaders)
{
    std::string request_str =
        "GET /dashboard/111/222/333?user=42&page=1 HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: "
        "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Cookie: session_id=abc123; theme=dark\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "\r\n";
    std::vector<std::string> param_names{"id1", "id2", "id3", "user", "page"};
    HttpRequest request{request_str, "/dashboard/{id1}/{id2}/{id3}?user={user}&page={page}",
                        param_names};
    ASSERT_EQ(request.get_header("User-Agent").value(),
              "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) "
              "Chrome/123.0.0.0 Safari/537.36");
    ASSERT_THROW(request.get_header("Mothefucker").value(), std::bad_optional_access);
    ASSERT_EQ(request.get_header("Connection").value(), "keep-alive");
}

TEST(HttpRequestParsing, PostRequestMethod)
{
    std::string request_str =
        "POST /api/login HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: application/json\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 44\r\n"
        "Cookie: session_id=abc123\r\n"
        "\r\n"
        "{\"username\": \"alice\", \"password\": \"secret\"}";
    HttpRequest request{request_str, "/api/login"};
    ASSERT_EQ(request.get_method(), RequestType::POST);
}

TEST(HttpRequestParsing, PostRequestBody)
{
    std::string request_str =
        "POST /api/login HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
        "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
        "Accept: application/json\r\n"
        "Accept-Language: en-US,en;q=0.9\r\n"
        "Accept-Encoding: gzip, deflate, br\r\n"
        "Connection: keep-alive\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 44\r\n"
        "Cookie: session_id=abc123\r\n"
        "\r\n"
        "{\"username\": \"alice\", \"password\": \"secret\"}";
    HttpRequest request{request_str, "/api/login"};
    ASSERT_TRUE(request.body_as_json());

    {
        std::string request_str =
            "POST /api/login HTTP/1.1\r\n"
            "Host: example.com\r\n"
            "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like "
            "Gecko) Chrome/123.0.0.0 Safari/537.36\r\n"
            "Accept: application/json\r\n"
            "Accept-Language: en-US,en;q=0.9\r\n"
            "Accept-Encoding: gzip, deflate, br\r\n"
            "Connection: keep-alive\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 44\r\n"
            "Cookie: session_id=abc123\r\n"
            "\r\n";
        HttpRequest request{request_str, "/api/login"};
        ASSERT_FALSE(request.body_as_json());
    }
}