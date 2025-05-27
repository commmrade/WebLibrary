#include "server/RequestType.hpp"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <ostream>
#include <server/Utils.hpp>
#include <iostream>
#include <print>

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

    // Step 1: Extract the base path up to the first "/" or "?"
    size_t first_delim = url.find_first_of("/?", 1);
    if (first_delim == std::string::npos) {
        result = std::string(url);
        return result.empty() ? "/" : result;
    }
    result = std::string(url.substr(0, first_delim));

    // Step 2: Process the remaining URL
    std::string_view remaining = url.substr(first_delim);

    // Step 3: Handle path parameters
    if (remaining[0] == '/') {
        while (!remaining.empty() && remaining[0] == '/') {
            remaining.remove_prefix(1); // Skip the '/'
            size_t next_delim = remaining.find_first_of("/?");
            std::string_view segment = (next_delim == std::string::npos) ? remaining : remaining.substr(0, next_delim);

            if (!segment.empty()) {
                result += "/{}"; // Replace path segment with "{}"
            }

            remaining.remove_prefix(next_delim == std::string::npos ? remaining.size() : next_delim);
        }
    }

    // Step 4: Handle query parameters
    if (!remaining.empty() && remaining[0] == '?') {
        result += "?";
        remaining.remove_prefix(1); // Skip the '?'
        bool first_param = true;

        while (!remaining.empty()) {
            size_t equal_pos = remaining.find('=');
            if (equal_pos == std::string::npos) break; // Malformed query, stop processing

            std::string_view key = remaining.substr(0, equal_pos);
            if (key.empty()) break; // No key, stop processing

            remaining.remove_prefix(equal_pos + 1); // Skip key and '='
            size_t end_pos = remaining.find('&');
            // Skip the value
            remaining.remove_prefix(end_pos == std::string::npos ? remaining.size() : end_pos);

            // Append key with "{}" as value
            if (!first_param) {
                result += "&";
            }
            result += std::string(key) + "={}";
            first_param = false;

            if (end_pos != std::string::npos) {
                remaining.remove_prefix(1); // Skip '&'
            }
        }
    }

    return result.empty() ? "/" : result;
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
