#include "server/RequestType.hpp"
#include <filesystem>
#include <server/Utils.hpp>
#include <iostream>

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
    // Process route and add all key values to the parameters map
    std::string result;
    result += url.substr(0, url.find("/", 1) != std::string::npos ? url.find("/", 1) : url.find("?"));
    
    if (result.contains("public") || result.contains("private") || result.contains("static")) {
       
        return std::string{url};
    }

    { // Path parameter parsing
        std::string_view request_url = url.substr(1);  

        if (request_url.find("?") == std::string::npos || request_url.find("/") == std::string::npos) { // If without query
            return result;
        }

        if (request_url.find("/") != std::string::npos) { // Parsing path parameters like /{id}/{smth}?name={val}
            size_t pos;
            while ((pos = request_url.find("/")) != std::string::npos) {
                size_t end_pos = (request_url.find("/", pos + 1) == std::string::npos) ? request_url.find("?") : request_url.find("/", pos + 1);

                result += "/{}"; 
                request_url.remove_prefix(end_pos);
            }
        } else {
            request_url.remove_prefix(request_url.find("?"));
        }
        

        if (request_url.find("?") != std::string::npos) {  // Parsing ?name={val}
          
            size_t start_pos = request_url.find_first_not_of("?");
            size_t end_pos = request_url.find("&") != std::string::npos ? request_url.find("&") : request_url.size();
            
            std::string_view key_value = request_url.substr(start_pos, end_pos - start_pos - 1);

            std::string_view name = key_value.substr(0, key_value.find("="));
            result += "?" + std::string{name} + "=" + "{}";
            
            request_url.remove_prefix(end_pos);
            
            while (request_url.find("&") != std::string::npos) { // Parsing &name={val}&...
                size_t start_pos = request_url.find_first_not_of("&");
                size_t end_pos = request_url.find("&", 1);

                std::string_view key_value = request_url.substr(start_pos, end_pos - start_pos - 1);
                std::string_view name = key_value.substr(0, key_value.find("="));
                result += "&" + std::string{name} + "=" "{}";
                
                request_url.remove_prefix(end_pos == std::string::npos ? request_url.size() : end_pos);
            }
            
        }

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

std::string find_file(const std::string &filename) {
    if (std::filesystem::exists("static/public/" + filename)) {
        return "public/";
    } else if (std::filesystem::exists("static/private/" + filename)) {
        return "private/";
    }
    return "";
}

}
