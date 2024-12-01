#include <stdexcept>
#include<string>
#include<unordered_map>
#include<optional>
#include<sstream>

class response {
private:
    int status;
    std::unordered_map<std::string, std::string> headers;
    std::string headers_raw;
    std::string body;
    
public:
    response(int status_code, const std::string &headers, const std::string &body);
    
    inline std::string text() { return body; }
    inline int status_code() { return status; }

    auto get_header(const std::string &name) -> std::optional<std::string>;
    
private:
    void extract_headers();
};
