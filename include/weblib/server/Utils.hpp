// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once


#include "weblib/server/RequestType.hpp"
#include <string>
#include <vector>



namespace utils {
auto req_type_from_str(std::string_view str) -> RequestType;

auto process_url_str(std::string_view url) -> std::string;


auto extract_params(std::string_view url) -> std::vector<std::string>;

auto to_lowercase_str(std::string str) -> std::string;

void trim_r(std::string &str);
void trim_l(std::string &str);
void trim(std::string &str);

}
