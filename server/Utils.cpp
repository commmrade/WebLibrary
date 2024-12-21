#include <server/Utils.hpp>



RequestType req_type_from_str(const std::string &str) {
    if (str == "GET") {
        return RequestType::GET;
    } else if (str == "POST") {
        return RequestType::POST;
    } else {
        return RequestType::GET;
    }
}


std::string process_url_str(const std::string &url) {
    if (url.find("?") == url.npos) {
        return url;
    }

    std::string base_url = url.substr(0, url.find("/") );
    
    std::string query_part = url.substr(url.find("?") + 1);

    do {
        std::string part_query = query_part.substr(0, query_part.find("&")); // Getting key=value out of it

        std::string key = part_query.substr(0, part_query.find("=") + 1); // Getting key
        
        base_url += key + "?";
        if (query_part.find("&") != std::string::npos) { // If there are more query parts add &
            base_url += '&';
        }

        auto end_pos = query_part.find("&");
        query_part = query_part.substr((end_pos == query_part.npos) ? (query_part.size()) : (end_pos + 1));
    } while (!query_part.empty());
    
    return url.substr(0, url.find("?") + 1) + base_url;

}


void trim_r(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [] (auto &elem) { return !isspace(elem); }).base(), s.end());
}
void trim_l(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char elem) { return !std::isspace(elem); }));
}

void trim(std::string &s) {
    trim_l(s);
    trim_r(s);
}
