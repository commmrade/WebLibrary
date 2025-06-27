#include "weblib/requests/request.hpp"


Request::Request(const std::string &url_path_p, int port = 80) {
    process_route(url_path_p);
    estabilish_connection(port);
}


Request::~Request() {
    close(sock);
    freeaddrinfo(adres);
}


void Request::process_route(std::string url_path) {
    if (url_path.find("https://") != url_path.npos) {
        url_path.replace(url_path.find("https://"), 8, "");

    } else if (url_path.find("http://") != url_path.npos) {
        url_path.replace(url_path.find("http://"), 7, "");

    }
    host = url_path.substr(0, url_path.find("/"));
    api = url_path.substr(url_path.find("/"));
}


void Request::estabilish_connection(int port) {
    int getaddr = getaddrinfo(host.c_str(), std::to_string(port).c_str(), NULL, &adres); // Getting ip from DNS

    if (getaddr != 0) {
        perror("Addr failed");
        exit(0);
    }

    sock = socket(adres->ai_family, adres->ai_socktype, adres->ai_protocol); // Creating a socket
    if (sock < 0) {
        perror("Socket error");
        exit(0);
    }

    int connection = connect(sock, adres->ai_addr, adres->ai_addrlen); // Making a connection to the server
    if (connection < 0) {
        perror("Connection error");
        exit(0);
    }
}

void Request::send(const std::string &request) {
    int sent = ::send(sock, request.c_str(), request.size(), 0);
    if (sent < 0) {
        perror("Sending error");
        exit(0);
    }
}

auto Request::receive() -> response {
    std::string result;
    result.resize(4096);

    size_t already_read{0};
    size_t rd_bytes;
    while ((rd_bytes = read(sock, result.data() + already_read, 4096)) > 0) {
        already_read += rd_bytes;

        if (already_read >= result.size()) {
            result.resize(result.size() * 2);
        }
    }
    result.resize(already_read);
    result.shrink_to_fit();
    

    return parse_raw_response(result);
}


auto Request::parse_raw_response(const std::string &buf) -> response {
    std::stringstream ss{buf};

    int status_code;
    std::string http_ver;
    std::string status_message;

    std::string headers_raw;
    std::string body;

    std::getline(ss, http_ver, ' ');
    ss >> status_code;
    std::getline(ss, status_message);


    std::string line;
    while (std::getline(ss, line) && !(line == "\r")) { // Extracting the response headers
        headers_raw += line += '\n';
    }
    
    body += line;
    while (std::getline(ss, line)) { // Extracting the response body
        body += line;
    }
    
    return response{status_code, headers_raw, body};
}