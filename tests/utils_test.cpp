#include <server/Utils.hpp>
#include <gtest/gtest.h>


TEST(Utils, ProcessUrlStrTest) {
    std::string target = "/smth/208?name=pidor";
    auto processed_str = utils::process_url_str(target);
    ASSERT_EQ(processed_str, "/smth/{}?name={}");
    {
        std::string target = "/smth/208/209?name=pidor&faggot=true";
        auto processed_str = utils::process_url_str(target);
        ASSERT_EQ(processed_str, "/smth/{}/{}?name={}&faggot={}");
    }
    {
        std::string target = "/smth/208/209?name=pidor&cock=true&moron=true&russia=fag";
        auto processed_str = utils::process_url_str(target);
        ASSERT_EQ(processed_str, "/smth/{}/{}?name={}&cock={}&moron={}&russia={}");
    }
}

TEST(Utils, ExtractParamsTest) {
    auto url = std::string{"/smth/{id1}/{id2}?name={name}&cock={cock}&moron={moron}&russia={russia}"};
    auto processed = utils::extract_params(url);
    ASSERT_EQ(processed[0], "id1"); ASSERT_EQ(processed[1], "id2"); ASSERT_EQ(processed[2], "name"); ASSERT_EQ(processed[3], "cock"); ASSERT_EQ(processed[4], "moron");
    ASSERT_EQ(processed[5], "russia");
}
