#include <cerrno>
#include <exception>
#include <iostream>
#include <istream>
#include <memory>
#include <optional>
#include <random>
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
#include<boost/asio.hpp>

#include <cstdio>



class Response {
private:
    int status;
    std::unordered_map<std::string, std::string> headers;
    std::string headers_raw;
    std::string body;
    
public:
    Response(int status_code, const std::string &headers, const std::string &body) : headers_raw(headers), body(body), status(status_code) {
        extract_headers();
    }
    
    std::string text() {
        return body;
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
   
    void extract_headers() {
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
    
};

class Client {
protected:
    std::string url;
    std::unordered_map<std::string, std::string> headers;
    std::string request_str;
    std::string body;
public:
    virtual std::string prepare_request_str() = 0;
    void set_header(const std::string &key, const std::string &value) {
        headers[key] = value;
    }
    std::string get_url() { return url; }
};

class GetClient final : public Client {
public:
    GetClient(const std::string &url_) {
        url = url_;
    }
    std::string prepare_request_str() override {

        if (url.find("https://") != url.npos) {
            url.replace(url.find("https://"), 8, "");
        } else if (url.find("http://") != url.npos) {
            url.replace(url.find("http://"), 7, "");
        }
        std::string host = url.substr(0, url.find("/"));
        std::string api = url.substr(url.find("/"));
        

        std::string request_query{};
        request_query += "GET " + api + " HTTP/1.1\r\n";
        request_query += "Host: " + host + "\r\n";
        for (auto start = headers.begin(); start != headers.end(); start++) {
            request_query += (*start).first + ": " + (*start).second;
        }
        request_query += "Connection: close\r\n\r\n";

        return request_query;
    }
};
class PostClient final : public Client {
private:
    std::string body;
public:
    PostClient(const std::string &url_) {
        url = url_;

    }
    std::string prepare_request_str() override {
        if (url.find("https://") != url.npos) {
            url.replace(url.find("https://"), 8, "");
        } else if (url.find("http://") != url.npos) {
            url.replace(url.find("http://"), 7, "");
        }
        std::string host = url.substr(0, url.find("/"));
        std::string api = url.substr(url.find("/"));
        

        std::string request_query{};
        request_query += "POST " + api + " HTTP/1.1\r\n";
        request_query += "Host: " + host + "\r\n";
        for (auto start = headers.begin(); start != headers.end(); start++) {
            request_query += (*start).first + ": " + (*start).second;
        }
        request_query += "Connection: close\r\n\r\n";
        request_query += body;

        return request_query;
    }
    void set_body(const std::string &str) {
        body = str;
    }
};


class Request {
private:
   
    
    std::string host, api;

    addrinfo *adres;
    int sock;

public:
    Request(const std::string &url_path_p, int port = 80) {
        process_route(url_path_p);
        estabilish_connection(port);
    }
    
    ~Request() {
        close(sock);
        freeaddrinfo(adres);
    }
    
    static Response execute(Client &client) {
        Request req(client.get_url());
        req.send(client.prepare_request_str());
        return req.receive();
    }

    
private:

    void process_route(std::string url_path) {
         if (url_path.find("https://") != url_path.npos) {
            url_path.replace(url_path.find("https://"), 8, "");
        } else if (url_path.find("http://") != url_path.npos) {
            url_path.replace(url_path.find("http://"), 7, "");
        }
        host = url_path.substr(0, url_path.find("/"));

        api = url_path.substr(url_path.find("/"));
    }

    void estabilish_connection(int port) {
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


    void send(const std::string &request) {
        int sent = ::send(sock, request.c_str(), request.size(), 0);
        if (sent < 0) {
            perror("Sending error");
            exit(0);
        }
    }
    Response receive() {
        std::string buf = [this] -> std::string {
            std::string result;
            result.reserve(4096);

            int already_read{};
            while (true) { // Reading whole request
                result.resize(already_read + 1);
                
                size_t rd_bytes = read(sock, result.data() + already_read, 1);

                if (rd_bytes == 0) {
                    break;
                } else if (rd_bytes < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        std::cout << "Data end\n";
                        break;
                    }
                    std::cerr << "error reading\n";
                    break;
                }
                already_read += rd_bytes;
            }

            return result;
        }();
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
        
        return Response{status_code, headers_raw, body};
    }
};