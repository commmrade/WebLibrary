#include <exception>
#include <iostream>
#include <istream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <system_error>
#include<unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unordered_map>
#include<boost/asio.hpp>

#include <cstdio>



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

class Request {
private:
    ConnectionType type;
    std::string url_path;
    std::string host, api;

    boost::asio::io_service io_service;

    std::unique_ptr<boost::asio::ip::tcp::socket> socket;

public:
    Request(ConnectionType type, const std::string &url_path) : type(type), url_path(url_path) {
        host = url_path.substr(0, url_path.find("/"));
        api = url_path.substr(url_path.find("/"));
        


        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, "http");
        boost::asio::ip::tcp::resolver::results_type endpoint_iter = resolver.resolve(query);

        socket = std::make_unique<boost::asio::ip::tcp::socket>(io_service);
    
        boost::asio::connect(*socket, endpoint_iter);
    }
    Request(ConnectionType type, std::string &&url_path) : type(type), url_path(url_path) {
        host = url_path.substr(0, url_path.find("/"));
        api = url_path.substr(url_path.find("/"));
        


        boost::asio::ip::tcp::resolver resolver(io_service);
        boost::asio::ip::tcp::resolver::query query(host, "http");
        boost::asio::ip::tcp::resolver::results_type endpoint_iter = resolver.resolve(query);

        socket = std::make_unique<boost::asio::ip::tcp::socket>(io_service);
    
        boost::asio::connect(*socket, endpoint_iter);
    }  
    
    ~Request() {
    }
    
    static Response get(const std::string &url) {
        Request request(GET, url);
        return request.connect_and_get_reply();
    }

    
private:
    void init() {
    }
    Response connect_and_get_reply() {
        switch (type) {
            case GET: {
              
                std::string request_query{};
                request_query += "GET " + api + " HTTP/1.1\r\n";
                request_query += "Host: " + host + "\r\n";
                request_query += "Connection: close\r\n\r\n";

                
                boost::asio::write(*socket, boost::asio::buffer(request_query));
               
                boost::asio::streambuf response;
                boost::asio::read_until(*socket, response, "\r\n");

                int status_code;
                std::string http_ver;
                std::string status_message;

                std::string headers_raw;

                std::istream i_str(&response); 
                std::getline(i_str, http_ver, ' ');
                i_str >> status_code;
                std::getline(i_str, status_message);
                

                //TURN IT INTO READ THE REST...
                boost::asio::read_until(*socket, response, "\r\n\r\n");


                std::string hdr;
                while (std::getline(i_str, hdr)) {
                    
                    headers_raw += hdr + '\n';
                }
                

                return std::move(Response{status_code, headers_raw.substr(0, headers_raw.find("\r\n\r\n")), headers_raw.substr(headers_raw.find("\r\n\r\n") + 4, headers_raw.find_last_of("\r\n\r\n") - (headers_raw.find("\r\n\r\n") + 4))});

                break;
            }
            default: {
                std::cerr << "Only GET is implemented\n";
                
                exit(-1);
            }
        }
        
    }
};