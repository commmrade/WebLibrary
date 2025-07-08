#include "weblib/requests/response.hpp"


response::response(int status_code, const std::string &headers, const std::string &body) : status(status_code), headers_raw(headers), body(body) {
    extract_headers();
}

auto response::get_header(const std::string &name) -> std::optional<std::string> {
    auto pos = headers.find(name);
    if (pos != headers.end()) {
        return std::optional<std::string>{(*pos).second};
    }
    return std::nullopt;
}

void response::extract_headers() {
    std::string headers_section = std::move(headers_raw);

    if (headers_section.empty()) {
        throw std::runtime_error("Headers are missing..");
    }

    std::stringstream ss(headers_section);

    std::string line;
    while (std::getline(ss, line)) {
        if (line.find("\r") != line.npos) {
            line.pop_back();
        }
        std::string name = line.substr(0, line.find(":"));
        std::string value = line.substr(name.size() + 2);; 
        headers[name] = value;
    }
}