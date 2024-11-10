#include <boost/asio/completion_condition.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/read.hpp>
#include <boost/system/detail/error_code.hpp>
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

class GetClient : public Client {
public:
    GetClient(const std::string &url_) {
        url = url_;
    }
    std::string prepare_request_str() override {

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



class Request {
private:
    ConnectionType type;
    std::string url_path;
    std::string host, api;

    boost::asio::io_service io_service;

    std::unique_ptr<boost::asio::ip::tcp::socket> socket;

public:
    Request(const std::string &url_path) : type(type), url_path(url_path) {
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
    
    static Response execute(Client &client) {
        Request req(client.get_url());
        req.send(client.prepare_request_str());
        return req.receive();
    }

    
private:
    void send(const std::string &request) {
        boost::asio::write(*socket, boost::asio::buffer(request));
    }
    
    Response receive() {
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
        

      
        boost::system::error_code error_code;
        while (boost::asio::read(*socket, response, boost::asio::transfer_at_least(1), error_code)) {
            if (error_code != boost::asio::error::eof) {
                std::cout << error_code.what() << std::endl;
            }
        }

        std::string hdr;
        while (std::getline(i_str, hdr)) {
            headers_raw += hdr + '\n';
        }
        
        return std::move(Response{status_code, headers_raw.substr(0, headers_raw.find("\r\n\r\n")), headers_raw.substr(headers_raw.find("\r\n\r\n") + 4, headers_raw.find_last_of("\r\n\r\n") - (headers_raw.find("\r\n\r\n") + 4))});
    }
};