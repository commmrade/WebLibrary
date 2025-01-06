#include <server/HttpRequest.hpp>

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

    auto key_name_iter = param_names.begin();


    { // Path parameter parsing
        auto request_url = request.substr(request.find("/") + 1, request.find("HTTP") - request.find("/") - 2);   
        
        if (param_names.size() == 0) { // iF weithout query
            return;
        }


        if (request_url.find("/") != std::string::npos) {
            size_t pos;
            while ((pos = request_url.find("/")) != std::string::npos) {

                auto temp = request_url.substr(request_url.find("/"));

                auto start_pos = pos;
                auto end_pos = (request_url.find("/", pos + 1) == std::string::npos) ? request_url.find("?") : request_url.find("/", pos + 1);

                auto value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);
                
                if (key_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
                auto key_name = *key_name_iter;
                key_name_iter++; 

                parameters[key_name] = value;
                


                request_url = request_url.substr(end_pos);
            }
        }

        if (request_url.find("?") != std::string::npos) { // ?name=fuck&password=ass
            auto start_pos = request_url.find_first_not_of("?");
            auto end_pos = request_url.find("&") != std::string::npos ? request_url.find("&") : request_url.size();
            
            auto key_value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);

            auto value = key_value.substr(key_value.find("=") + 1);
            
            if (key_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
            auto name = *key_name_iter;
        
            key_name_iter++; 
            parameters[name] = value;
        
            request_url = request_url.substr(end_pos);
            
            while (request_url.find("&") != std::string::npos) {
                auto start_pos = request_url.find_first_not_of("&");
                auto end_pos = request_url.find("&");
                
                auto key_value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);

                if (key_name_iter == param_names.end()) throw std::runtime_error("Malformed http request");
                auto name = *key_name_iter;
                
                key_name_iter++;

                auto value = key_value.substr(key_value.find("=") + 1);
                parameters[name] = value;
                
                request_url = request_url.substr(end_pos == 0 ? request_url.size() : end_pos);
            }
            
        }


    }
}


void HttpRequest::extract_headers() {
    
    std::string headers_cont = request.substr(request.find("\r\n") + 2, request.find("\r\n\r\n") - (request.find("\r\n") + 2));

    if (headers_cont.empty()) {
        std::cerr << "Headers not found\n";
        return;
    }

    std::stringstream strm(headers_cont);
    std::string line;
    while (std::getline(strm, line, '\n')) { // Extracting headers one by one
        if (line.find("\r") != line.npos) { // Remove \r if it is in the line
            line.pop_back();
        } 
        
        std::string name = line.substr(0, line.find(":"));
        std::string value = line.substr(name.size() + 2);; 
        
        if (name != "Cookie") {
            headers[name] = value;

        } else { // Extracting cookies
            std::string name = line.substr(0, line.find(":"));
            std::string vals_str = line.substr(line.find(":") + 2);

            while (!vals_str.empty()) {
                auto next_pos = vals_str.find(";");

                // Extracting name and value
                auto eq_pos = vals_str.find("="); 
                if (eq_pos == std::string::npos) {
                    std::cerr << "Invalid cookie format: " << vals_str << std::endl;
                    break;
                }

                std::string cookie_name = vals_str.substr(0, eq_pos);
                std::string value = (next_pos == std::string::npos) 
                                        ? vals_str.substr(eq_pos + 1) 
                                        : vals_str.substr(eq_pos + 1, next_pos - eq_pos - 1);

        
                utils::trim(cookie_name);
                utils::trim(value);
                
                cookies.insert({cookie_name, Cookie{cookie_name, value}});

                // Update vals_str for the next iteration
                if (next_pos == std::string::npos) {
                    vals_str.clear(); // No more cookies
                } else {
                    vals_str = vals_str.substr(next_pos + 1);
                }
            }
        }

    }
    
}