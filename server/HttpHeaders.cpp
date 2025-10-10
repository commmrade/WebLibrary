#include "weblib/server/HttpHeaders.hpp"
#include "weblib/debug.hpp"
#include "weblib/exceptions.hpp"
#include "weblib/utils.hpp"
#include <algorithm>
#include <stdexcept>
#include <ranges>
#include "weblib/consts.hpp"
namespace weblib {
void HttpHeaders::extract_headers_from_str(const std::string &raw_headers)
{
    if (raw_headers.empty())
    {
        return;
    }
    std::istringstream strm(raw_headers); 
    std::string        header;
    while (std::getline(strm, header, '\n'))
    {
        utils::trim(header); // get rid of carriage return

        auto pos = header.find(':');
        if (pos == std::string::npos)
        {
            debug::log_error("Could not find a ':' in the header");
            throw header_parsing_error{};
        }

        auto name  = std::string(header.begin(),
                                 header.begin() + static_cast<std::string::difference_type>(pos));
        auto value = std::string(header.begin() + static_cast<std::string::difference_type>(pos) + 2,
                                 header.end());
        utils::trim(value);

        auto lc_name = utils::to_lowercase_str(name);
        if (lc_name != HeaderConsts::COOKIE_HEADER)
        {
            m_headers.emplace(lc_name, std::move(value));
        }
        else
        {
            auto values = value | std::views::split(';') |
                          std::views::transform(
                              [](auto &&range)
                              {
                                  auto val = std::string{range.begin(), range.end()};
                                  utils::trim(val);
                                  return val;
                              }) |
                          std::ranges::to<std::vector<std::string>>();
            std::ranges::for_each(values,
                                  [&](auto &&cookie)
                                  {
                                      auto name_value = cookie | std::views::split('=') |
                                                        std::ranges::to<std::vector<std::string>>();
                                      if (name_value.size() != 2)
                                      {
                                          throw header_parsing_error{};
                                      }
                                      auto const name  = std::move(name_value.front());
                                      auto const value = std::move(name_value.back());
                                      m_cookies.emplace(
                                          utils::to_lowercase_str(name),
                                          Cookie{std::move(name), std::move(value)});
                                  });
        }
    }
}
auto HttpHeaders::get_cookie(const std::string &name) const -> std::optional<Cookie>
{
    auto pos = m_cookies.find(utils::to_lowercase_str(name));
    if (pos != m_cookies.end())
    {
        return std::optional<Cookie>{pos->second};
    }
    return std::nullopt;
}

auto HttpHeaders::get_header(const std::string &header_name) const -> std::optional<std::string>
{
    auto pos = m_headers.find(utils::to_lowercase_str(header_name));
    if (pos != m_headers.end())
    { 
        return std::optional<std::string>{pos->second};
    }
    return std::nullopt;
}}
 // namespace weblib}