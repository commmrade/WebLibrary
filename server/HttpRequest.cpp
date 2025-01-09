#include "server/Utils.hpp"
#include <server/HttpRequest.hpp>
#include <iostream>

HttpRequest::HttpRequest(const std::string &resp, std::span<const std::string> vec) : request(resp), param_names(std::vector<std::string>{vec.begin(), vec.end()}) {
    extract_headers();
    extract_queries();
}

[[nodiscard]]
Query HttpRequest::get_query(const std::string& query_name) const {
    Query query;
    auto pos = parameters.find(query_name);
    if (pos != parameters.end()) { //If header exists
        query.content = pos->second;
    }
    return query;
}

[[nodiscard]]
std::optional<std::string> HttpRequest::get_header(const std::string &header_name) const {
    auto pos = headers.find(header_name);
    if (pos != headers.end()) { //If header exists
        return pos->second;
    }
    return std::nullopt;
}


[[nodiscard]]
std::optional<Cookie> HttpRequest::get_cookie(const std::string &name) const {
    auto pos = cookies.find(name);
    if (pos != cookies.end()) {
        return pos->second;
    }
    return std::nullopt;
}

[[nodiscard]]
std::unique_ptr<Json::Value> HttpRequest::body_as_json() const {
    const std::string raw_json = request.substr(request.find("\r\n\r\n") + 4);
    Json::Value json_obj;


    Json::Reader json_reader;
    if (!json_reader.parse(raw_json, json_obj)) {
        return nullptr;
    }
    return std::make_unique<Json::Value>(std::move(json_obj)); // Hopefuly Json::Value is good at move semantics
}

void HttpRequest::extract_queries() {

    auto param_name_iter = param_names.begin(); // param_names stores id, user from api/{id}/{user} (example)
    {
        //std::string request_url = request.substr(request.find("/") + 1, request.find("HTTP") - request.find("/") - 2);   
        std::string_view request_url{request.data() + request.find("/") + 1, request.find("HTTP") - request.find("/") - 2};

        if (param_names.size() == 0) { // iF weithout query
            return;
        }
        if (request_url.find("/") != std::string::npos) { // Parsing api/{id}/{user} params
            size_t pos;
            while ((pos = request_url.find("/")) != std::string::npos) {
                size_t start_pos = pos;
                size_t end_pos = (request_url.find("/", pos + 1) == std::string::npos) ? request_url.find("?") : request_url.find("/", pos + 1);

                std::string_view value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);
                
                if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
                parameters.emplace(*param_name_iter, std::string(value));
                param_name_iter++; 

                request_url.remove_prefix(end_pos);
            }
        }

        if (request_url.find("?") != std::string::npos) { // ?name={smth} parsing 
            size_t start_pos = request_url.find_first_not_of("?");
            size_t end_pos = request_url.find("&") != std::string::npos ? request_url.find("&") : request_url.size();
            
            std::string_view key_value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);
            
            if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
            auto value = key_value.substr(key_value.find("=") + 1);
            parameters.emplace(*param_name_iter, std::string(value)); 
            param_name_iter++; 

            request_url.remove_prefix(end_pos);            
            while (request_url.find("&") != std::string::npos) { // &name={smth} parsing
                size_t start_pos = request_url.find_first_not_of("&");
                size_t end_pos = request_url.find("&");
                
                std::string_view key_value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);

                if (param_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
                std::string_view value = key_value.substr(key_value.find("=") + 1);
                parameters.emplace(*param_name_iter, std::string(value));
                param_name_iter++;

                request_url.remove_prefix(end_pos == 0 ? request_url.size() : end_pos);
            }   
        }
    }
}


void HttpRequest::extract_headers() {
    

    

    std::stringstream strm(request.substr(request.find("\r\n") + 2, request.find("\r\n\r\n") - (request.find("\r\n") + 2))); // here
    
    if (strm.view().empty()) {
        std::cerr << "Headers not found\n";
        return;
    }
    std::string line;
    while (std::getline(strm, line, '\n')) { // Extracting headers one by one
        if (line.find("\r") != line.npos) { // Remove \r if it is in the line
            line.pop_back();
        } 
        
        std::string name = line.substr(0, line.find(":"));
        std::string value = line.substr(name.size() + 2);; 
        
        if (name != "Cookie") {
            headers.emplace(std::move(name), std::move(value)); // Add header
        } else { // If it is a cookie
           
            std::string_view vals_str{line.begin() + (line.find(":") + 2), line.end()};

            while (!vals_str.empty()) {
                auto next_pos = vals_str.find(";");

                // Extracting name and value
                auto eq_pos = vals_str.find("="); 
                if (eq_pos == std::string::npos) {
                    std::cerr << "Invalid cookie format: " << vals_str << std::endl;
                    break;
                }

                std::string cookie_name = std::string{vals_str.substr(0, eq_pos)};
                std::string value = (next_pos == std::string::npos) 
                                        ? std::string{vals_str.substr(eq_pos + 1)} 
                                        : std::string{vals_str.substr(eq_pos + 1, next_pos - eq_pos - 1)};

                utils::trim(cookie_name);
                utils::trim(value);
                
                cookies.emplace(cookie_name, Cookie{cookie_name, std::move(value)});
                // Update vals_str for the next iteration
                if (next_pos == std::string::npos) {
                    vals_str.remove_prefix(vals_str.size());
                } else {
                    vals_str.remove_prefix(next_pos + 1);
                }
            }
        }

    }
    
}