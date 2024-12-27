#include <server/Utils.hpp>
#include <iostream>



RequestType req_type_from_str(const std::string &str) {
    if (str == "GET") {
        return RequestType::GET;
    } else if (str == "POST") {
        return RequestType::POST;
    } else {
        return RequestType::GET;
    }
}


// std::string process_url_str(const std::string &url) {
//     if (url.find("?") == url.npos) {
//         return url;
//     }

//     std::string base_url = url.substr(0, url.find("/") );
    
//     std::string query_part = url.substr(url.find("?") + 1);

//     do {
//         std::string part_query = query_part.substr(0, query_part.find("&")); // Getting key=value out of it

//         std::string key = part_query.substr(0, part_query.find("=") + 1); // Getting key
        
//         base_url += key + "?";
//         if (query_part.find("&") != std::string::npos) { // If there are more query parts add &
//             base_url += '&';
//         }

//         auto end_pos = query_part.find("&");
//         query_part = query_part.substr((end_pos == query_part.npos) ? (query_part.size()) : (end_pos + 1));
//     } while (!query_part.empty());
    
//     return url.substr(0, url.find("?") + 1) + base_url;

// }



std::string process_url_str(const std::string &url) {

    std::string request = url;
    
    // Process route and add all key values to the parameters map
 
    std::string result;
    result += request.substr(0, request.find("/", 1) != std::string::npos ? request.find("/", 1) : request.find("?"));
   
    { // Path parameter parsing
        auto request_url = request.substr(1);  

        if (request_url.find("?") == std::string::npos || request_url.find("/") == std::string::npos) { // If without query
            return result;
        }

        if (request_url.find("/") != std::string::npos) {
            size_t pos;
            while ((pos = request_url.find("/")) != std::string::npos) {

                auto temp = request_url.substr(request_url.find("/"));

                auto start_pos = pos;
                auto end_pos = (request_url.find("/", pos + 1) == std::string::npos) ? request_url.find("?") : request_url.find("/", pos + 1);

                auto value = request_url.substr(start_pos + 1, end_pos - start_pos - 1);

               
                //request_url = request_url.replace(start_pos + 1, end_pos - start_pos - 1,"{" + *key_name_iter + "}");//insert way
                result += "/{}"; 
              

                request_url = request_url.substr(end_pos);
                //std::cout << request_url << std::endl;
            }
        } else {
            request_url = request_url.substr(request_url.find("?"));
        }
        

        if (request_url.find("?") != std::string::npos) { // ?name=fuck&password=ass
          
            auto start_pos = request_url.find_first_not_of("?");
            auto end_pos = request_url.find("&") != std::string::npos ? request_url.find("&") : request_url.size();
            
            auto key_value = request_url.substr(start_pos, end_pos - start_pos - 1);
            //td::cout << key_value << std::endl;
            auto name = key_value.substr(0, key_value.find("="));

          
            result += "?" + name + "=" + "{}";
            
            
            request_url = request_url.substr(end_pos);
            
            while (request_url.find("&") != std::string::npos) {
                auto start_pos = request_url.find_first_not_of("&");
                auto end_pos = request_url.find("&");

                auto key_value = request_url.substr(start_pos, end_pos - start_pos - 1);
                auto name = key_value.substr(0, key_value.find("="));

                // auto name = *key_name_iter;
                result += "&" + name + "=" "{}";
                
               

                auto value = key_value.substr(key_value.find("=") + 1);
                
                request_url = request_url.substr(end_pos == 0 ? request_url.size() : end_pos);
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
    trim_l(s);
    trim_r(s);
}

std::vector<std::string> extract_params(const std::string &url) {
    std::vector<std::string> key_names;

    {
        auto temp_route = url;
        while (temp_route.find("{") != std::string::npos && temp_route.find("}") != std::string::npos) {
            auto start_pos = temp_route.find("{");
            auto end_pos = temp_route.find("}");

            auto key_name = temp_route.substr(start_pos + 1, end_pos - start_pos - 1);
         
            key_names.push_back(key_name);

            temp_route = temp_route.substr(end_pos + 1);
        }
    }
    
    
    return key_names;
}
