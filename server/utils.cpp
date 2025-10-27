#include <algorithm>
#include <json/value.h>
#include <ranges>
#include "weblib/utils.hpp"
#include <cctype>
#include <stdexcept>
namespace weblib
{
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
                throw std::runtime_error("Ill-formed template path");
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

auto parse_request_line(std::string_view raw_http) -> std::pair<std::string, std::string>
{
    auto space_after_method = raw_http.find(' ');
    if (space_after_method == std::string_view::npos)
    {
        throw std::runtime_error("HTTP request is ill-formed");
    }
    auto method = raw_http.substr(0, space_after_method);

    auto space_after_path = raw_http.find(' ', space_after_method + 1);
    if (space_after_path == std::string_view::npos)
    {
        throw std::runtime_error("HTTP request is ill-formed");
    }

    std::string_view const path =
        raw_http.substr(space_after_method + 1, space_after_path - space_after_method - 1);
    return std::pair{std::string{method}, std::string{path}};
}

} // namespace utils

} // namespace weblib