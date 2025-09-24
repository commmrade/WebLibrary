#include "weblib/server/Utils.hpp"
#include "weblib/server/HttpHeaders.hpp"
#include <algorithm>
#include <iostream>
#include <ranges>


auto HttpHeaders::get_header(const std::string &header_name) const -> std::optional<std::string> {
    auto pos = m_headers.find(utils::to_lowercase_str(header_name));
    if (pos != m_headers.end()) { //If header exists
        return std::optional<std::string>{pos->second};
    }
    return std::nullopt;
}
