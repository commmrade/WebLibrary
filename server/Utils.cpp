#include "server/RequestType.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <ostream>
#include <server/Utils.hpp>
#include <iostream>
#include <print>
#include <stdexcept>

namespace utils {


RequestType req_type_from_str(std::string_view str) {
    if (str == "GET") {
        return RequestType::GET;
    } else if (str == "POST") {
        return RequestType::POST;
    } else if (str == "OPTIONS") {
        return RequestType::OPTIONS;
    } else if (str == "PUT") {
        return RequestType::PUT;
    } else if (str == "DELETE") {
        return RequestType::DELETE;
    } else {
        return RequestType::DELETE;
    }   
}


std::string process_url_str(std::string_view url) {
    std::string result;

    size_t idx{0}; // Pos of pointer
    while (idx < url.size()) {
        if (url[idx] == '{') {
            auto end_bracket_pos = url.find('}', idx + 1);
            if (end_bracket_pos == std::string::npos) {
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

void trim_r(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [] (auto &elem) { return !isspace(elem); }).base(), s.end());
}
void trim_l(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char elem) { return !std::isspace(elem); }));
}

void trim(std::string &s) {
    utils::trim_l(s);
    utils::trim_r(s);
}

std::string to_lowercase_str(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char ch) { return std::tolower(ch); });
    return str;
}

std::vector<std::string> extract_params(std::string_view url) {
    std::vector<std::string> key_names;

    while (url.find("{") != std::string::npos && url.find("}") != std::string::npos) {
        size_t start_pos = url.find("{");
        size_t end_pos = url.find("}");

        key_names.emplace_back(url.substr(start_pos + 1, end_pos - start_pos - 1));
        url.remove_prefix(end_pos + 1);
    }
    return key_names;
}


}
