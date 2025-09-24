#include "weblib/server/HttpHeaders.hpp"
#include "weblib/server/Utils.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <ranges>

void HttpHeaders::extract_headers_from_str(const std::string& request_str) {
    if (request_str.empty()) {
        return; // Empty headers
    }
    std::istringstream strm(request_str); // here
    std::string line;
    while (std::getline(strm, line, '\n')) { // Extracting headers one by one
        utils::trim(line); // Get rid of spaces and carriage returns

        auto pos = line.find(':');
        if (pos == std::string::npos) {
            std::cerr << "Malformed http m_request: no colon\n";
            throw std::runtime_error("Malformed http m_request 1");
        }

        auto name = std::string(line.begin(), line.begin() + static_cast<std::string::difference_type>(pos));
        auto value = std::string(line.begin() + static_cast<std::string::difference_type>(pos) + 2, line.end());
        utils::trim(value);

        auto lowercase_name = utils::to_lowercase_str(name);
        if (lowercase_name != "cookie") {
            m_headers.emplace(lowercase_name, std::move(value)); // Add header
        } else {
            auto values = value
                | std::views::split(';')
                | std::views::transform([](auto&& range) {
                    auto val = std::string{range.begin(), range.end()};
                    utils::trim(val);
                    return val;
                })
                | std::ranges::to<std::vector<std::string>>();
            std::ranges::for_each(values, [&](auto&& cookie) {
                auto name_value = cookie | std::views::split('=') | std::ranges::to<std::vector<std::string>>();
                if (name_value.size() != 2) {
                    throw std::runtime_error("Malformed http m_request");
                }
                auto const  name_cookie = std::move(name_value.front());
                auto const value_cookie = std::move(name_value.back());
                m_cookies.emplace(utils::to_lowercase_str(name_cookie), Cookie{std::move(name_cookie), std::move(value_cookie)});
            });
        }
    }
}
auto HttpHeaders::get_cookie(const std::string &name) const -> std::optional<Cookie> {
    auto pos = m_cookies.find(utils::to_lowercase_str(name));
    if (pos != m_cookies.end()) { //If header exists
        return std::optional<Cookie>{pos->second};
    }
    return std::nullopt;
}

auto HttpHeaders::get_header(const std::string &header_name) const -> std::optional<std::string> {
    auto pos = m_headers.find(utils::to_lowercase_str(header_name));
    if (pos != m_headers.end()) { //If header exists
        return std::optional<std::string>{pos->second};
    }
    return std::nullopt;
}