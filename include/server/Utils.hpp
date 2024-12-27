#pragma once
#include<algorithm>
#include <server/RequestType.hpp>
#include <string>
#include <vector>




RequestType req_type_from_str(const std::string &str);

std::string process_url_str(const std::string &url);


std::vector<std::string> extract_params(const std::string &url);

void trim_r(std::string &s);
void trim_l(std::string &s);
void trim(std::string &s);