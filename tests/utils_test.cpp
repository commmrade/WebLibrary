#include <weblib/server/Utils.hpp>
#include <gtest/gtest.h>
#include <string>

TEST(Utils, ProcessUrlStrTest)
{
    std::string target        = "/smth/{id}?name=pidor";
    auto        processed_str = utils::process_url_str(target);
    ASSERT_EQ(processed_str, "/smth/{}?name=pidor");
    {
        std::string target        = "/smth/{}/{}?name={name}&faggot={fag}";
        auto        processed_str = utils::process_url_str(target);
        ASSERT_EQ(processed_str, "/smth/{}/{}?name={}&faggot={}");
    }
    {
        std::string target =
            "/smth/{id1}/{id2}?name={name}&cock={cock}&moron={moron}&russia={fuck}";
        auto processed_str = utils::process_url_str(target);
        ASSERT_EQ(processed_str, "/smth/{}/{}?name={}&cock={}&moron={}&russia={}");
    }
    {
        std::string target        = "/fuck/{id}";
        auto        processed_str = utils::process_url_str(target);
        ASSERT_EQ(processed_str, "/fuck/{}");
    }
}

TEST(Utils, ProcessUrlSingleParam)
{
    std::string target        = "/{id}";
    auto        processed_str = utils::process_url_str(target);
    ASSERT_EQ(processed_str, "/{}");
}

TEST(Utils, ExtractParamsTest)
{
    auto url =
        std::string{"/smth/{id1}/{id2}?name={name}&cock={cock}&moron={moron}&russia={russia}"};
    auto processed = utils::extract_params(url);
    ASSERT_EQ(processed[0], "id1");
    ASSERT_EQ(processed[1], "id2");
    ASSERT_EQ(processed[2], "name");
    ASSERT_EQ(processed[3], "cock");
    ASSERT_EQ(processed[4], "moron");
    ASSERT_EQ(processed[5], "russia");
}

TEST(Utils, ToLowerCaseTest)
{
    std::string str = "FuCkMe";
    ASSERT_EQ(utils::to_lowercase_str(str), "fuckme");
}