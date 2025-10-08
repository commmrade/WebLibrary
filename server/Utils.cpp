#include <algorithm>
#include <json/value.h>
#include <ranges>

#include "weblib/server/Utils.hpp"
#include <cctype>
#include <print>
#include <stdexcept>

namespace utils
{
auto process_url_str(std::string_view url) -> std::string
{
    std::string result;

    size_t idx{0}; // Pos of pointer
    while (idx < url.size())
    {
        if (url[idx] == '{')
        {
            auto end_bracket_pos = url.find('}', idx + 1);
            if (end_bracket_pos == std::string::npos)
            {
                throw std::runtime_error("Malformed template path");
            }
            idx = end_bracket_pos + 1;
            result += "{}";
            continue;
        }

        result += url[idx];
        ++idx;
    }
    return result;
}

void trim_r(std::string &str)
{
    str.erase(std::ranges::find_if(std::ranges::views::reverse(str),
                                   [](auto &elem) { return !isspace(elem); })
                  .base(),
              str.end());
}
void trim_l(std::string &str)
{
    str.erase(str.begin(),
              std::ranges::find_if(str, [](unsigned char elem) { return !std::isspace(elem); }));
}

void trim(std::string &str)
{
    utils::trim_l(str);
    utils::trim_r(str);
}

auto error_response(std::string_view type, std::string_view message) -> Json::Value
{
    Json::Value resp;
    resp["type"]    = std::string{type};
    resp["message"] = std::string{message};
    return resp;
}

auto to_lowercase_str(std::string str) -> std::string
{
    std::ranges::transform(str, str.begin(), [](unsigned char cha) { return std::tolower(cha); });
    return str;
}

auto extract_params(std::string_view url) -> std::vector<std::string>
{
    std::vector<std::string> key_names;

    while (url.find('{') != std::string::npos && url.find('}') != std::string::npos)
    {
        size_t start_pos = url.find('{');
        size_t end_pos   = url.find('}');

        key_names.emplace_back(url.substr(start_pos + 1, end_pos - start_pos - 1));
        url.remove_prefix(end_pos + 1);
    }
    return key_names;
}

} // namespace utils
