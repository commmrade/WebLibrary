#include <cstdio>
#include <exception>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unordered_map>


enum ConnectionType {
    POST,
    GET,
    PUT,
    DELETE
};

class Response {
private:
    int status;
    std::unordered_map<std::string, std::string> headers;
    std::string raw_request;
public:
    Response(const std::string &req) : raw_request(req) {
        extract_headers();
        extract_status_code();
    }
    
    std::string text() {
        return raw_request.substr(raw_request.find("\r\n\r\n") + 4);
    }
    int status_code() {
        return status;
    }
    std::optional<std::string> get_header(const std::string &name) {
        auto pos = headers.find(name);
        if (pos != headers.end()) {
            return (*pos).second;
        }
        return std::nullopt;
    }
    
private:
    void extract_status_code() {
        std::string str = raw_request.substr(0, raw_request.find("\r\n"));
        std::string str_status_code = str.substr(str.find("1.1") + 4, str.find_last_of(" ") - (str.find("1.1") + 4));
        try {
            status = std::stoi(str_status_code);
        } catch (std::exception &exception) {
            std::cout << exception.what() << std::endl;
        }
    }
    void extract_headers() {
        std::string headers_section = raw_request.substr(raw_request.find("\r\n") + 2, raw_request.find("\r\n\r\n") - (raw_request.find("\r\n") + 2));

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
    
};

class Request {
private:
    ConnectionType type;
    std::string url_path;
    std::string host, api;

    addrinfo *adres;
    addrinfo hints = {0};

    int sock;

public:
    Request(ConnectionType type, const std::string &url_path) : type(type), url_path(url_path) {
        
        host = url_path.substr(0, url_path.find("/"));
        api = url_path.substr(url_path.find("/"));
        
        addrinfo hints = {0};

        int getaddr = getaddrinfo(host.c_str(), "80", &hints, &adres); //Getting ip from host name (str)

        if (getaddr != 0) {
            std::cerr << "Getting adres failed\n";
            perror("Addr failed ");
            exit(0);
        }

        sock = socket(adres->ai_family, adres->ai_socktype, adres->ai_protocol); 
        if (sock < 0) {
            std::cerr << "Error socket\n";
            exit(0);
        }

        int connection = connect(sock, adres->ai_addr, adres->ai_addrlen);
        if (connection < 0) {
            std::cerr << "Connection error\n";
            close(sock);
            exit(0);
        }
        

    }
    Request(ConnectionType type, std::string &&url_path) : type(type), url_path(url_path) {
        host = url_path.substr(0, url_path.find("/"));
        api = url_path.substr(url_path.find("/"));
        
        addrinfo hints = {0};

        int getaddr = getaddrinfo(host.c_str(), "80", &hints, &adres); //Getting ip from host name (str)

        if (getaddr != 0) {
            std::cerr << "Getting adres failed\n";
            perror("Addr failed ");
            exit(0);
        }

        sock = socket(adres->ai_family, adres->ai_socktype, adres->ai_protocol); //Creating socket
        if (sock < 0) {
            std::cerr << "Error socket\n";
            perror("Socket");
            exit(0);
        }

        int connection = connect(sock, adres->ai_addr, adres->ai_addrlen); //Connecting to ip address
        if (connection < 0) {
            std::cerr << "Connection error\n";
            perror("Connection");
            close(sock);
            exit(0);
        }

    }  
    ~Request() {
    }
    
    static Response get(const std::string &url) {
        Request request(GET, url);
        return request.connect_and_get_reply();
    }

    
private:
    
    Response connect_and_get_reply() {
        switch (type) {
            case GET: {
                std::string request_query{};
                request_query += "GET " + api + " HTTP/1.1\r\n";
                request_query += "Host: " + host + "\r\n";
                request_query += "Connection: close\r\n\r\n";

                int send_data = send(sock, request_query.c_str(), request_query.size(), 0); //Connecting to server
                if (send_data < 0) {
                    std::cerr << "Error sending\n";
                    perror("Sending ");
                    exit(0);
                }
                std::string result{};
                char buf[4096];
                while (true) {
                    ssize_t received_bytes = recv(sock, buf, sizeof(buf) - 1, 0); //Receiving info from server
                    if (received_bytes <= 0) {
                        break;
                    }
                    buf[received_bytes] = '\0';
                    result += buf;
                }
                Response resp(buf);

                close(sock);
                freeaddrinfo(adres);
                return resp;

                break;
            }
            default: {
                std::cerr << "Only GET is implemented\n";
                close(sock);
                freeaddrinfo(adres);
                exit(-1);
            }
        }
        
    }
};