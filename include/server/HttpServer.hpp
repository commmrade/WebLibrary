#pragma once

#include <algorithm>
#include <asm-generic/socket.h>
#include <cerrno>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <ostream>
#include <queue>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include<csignal>
#include<fcntl.h>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include "hash.hpp"
#include "ThreadPool.hpp"

enum class RequestType {
    POST,
    GET,
    PUT,
    DELETE
};


using Handler = std::function<void(HttpRequest&&, HttpResponse&&)>;
using Filter = std::function<bool(HttpRequest&&)>;

class HttpServer {
private:
    
    std::unordered_map<std::pair<std::string, RequestType>, Handler> endpoints;
    std::unordered_map<std::string, Filter> middlewares;
    int serv_socket;
    sockaddr_in serv_addr;

    std::shared_mutex mtx;

    inline static HttpServer *serv;

    bool is_running{false};

    ThreadPool thread_pool;

public: 

    ~HttpServer() {
        thread_pool.stop();
        close(serv_socket);
    }

    void register_handler(RequestType type, const std::string &endpoint_name, Handler handler) {
        endpoints[{endpoint_name, type}] = handler;
        middlewares[endpoint_name] = [] (auto &&req) {return true;};
        printf("hhhh\n");
    }

    
    void register_filter(const std::string &route, Filter filter) {
        middlewares[route] = filter;
    }

    
    static HttpServer& instance(int port = 8080) {
        if (serv == nullptr) {
            serv = new HttpServer(port);
        }
        return *serv;
    }

    bool is_ran() const { return is_running; }

    void listen_start() {
        is_running = true;

        if (endpoints.empty()) {
            throw std::runtime_error("Endpoints methods are not set");
        }

         while (true) {
            int client_socket = accept(serv_socket, nullptr, nullptr);
            if (client_socket < 0) {
                perror("Client socket");
                exit(-1);
            }
            
            thread_pool.add_job([this](int client_socket){ handle_incoming_request(client_socket); }, client_socket);

        }
    }
private:
    HttpServer(int port) {
        server_setup(port);

        thread_pool.create();

        signal(SIGINT, &HttpServer::sigint_handler);
    }


    void server_setup(int port) {
        serv_socket = socket(AF_INET, SOCK_STREAM, 0); // Creating a socket that can be connected to
        if (serv_socket < 0) {
            perror("Serv socket");
            exit(-1);
        }   

        int optval = 1;

        int sockopt = setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));// TODO: On release remove this
        if (sockopt < 0) { // TODO: On release remove this
            perror("Setting sockopt"); // TODO: On release remove this
            exit(-1); // TODO: On release remove this
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port); // Settings app addres info
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serv_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { // Binding socket
            std::cerr << "Error binding...\n";
            perror("Binding");
            exit(-1);
        } 


        if (listen(serv_socket, 999) < 0) { // Listening for incoming requests
            perror("Listening");
            exit(-1);
        }
    }

    void handle_incoming_request(int client_socket) {
        //std::this_thread::sleep_for(std::chrono::seconds(2));
     
        fcntl(client_socket, F_SETFL, O_NONBLOCK);
        std::string call;
        call.resize(4096);
        
      
        size_t already_read{0};
        int rd_bytes;
        while (true) {
            rd_bytes = read(client_socket, call.data() + already_read, 4096);
           
            if (rd_bytes > 0) {
                already_read += rd_bytes;   
                std::cout << rd_bytes << std::endl;

                if (already_read >= call.size()) {
                    call.resize(call.size() * 2);
                }

            } else if (rd_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                // Try again
        
            } else if (rd_bytes == -1) {
                break; // unknown error when reading
            }

            if (already_read > 0 && already_read < 4096) {
                break; // Got all data
            }

            
        }
        
        call.resize(already_read);
        call.shrink_to_fit();

       
    

        process_to_endpoint(client_socket, call);
        
        close(client_socket);
    }

    void process_to_endpoint(int client_socket, const std::string &call) {
        std::shared_lock lock{mtx};
      
        std::string method = call.substr(0, call.find("/") - 1); //Extracting method from request
        
        RequestType request_type = req_type_from_str(method);
        
        std::string api_route = call.substr(call.find(" ") + 1, call.find("HTTP") - (call.find(" ") + 2)); // URL path that was called like /api/HttpServer
        std::string base_url = process_url_str(api_route); //Replacing values with ?
       

        // TODO: SEPARATE ROUTING IN A SEPARATE UTILITY

        HttpResponse resp(client_socket);
        HttpRequest req(call);
        

        
        try { // If user function throws an exception the server doesn't crash
            if (endpoints.find({base_url, request_type}) != endpoints.end()) { // If such api exist

                if (middlewares[base_url](std::move(req))) {
                    endpoints.at({base_url, request_type})(std::move(req), std::move(resp));
                } else {
                    HttpResponse(client_socket).write_str("Access denied", 401);
                }
                
            } else {
                
                auto error_404 = [](auto &&req, auto &&resp) {
                    resp.set_header_raw("Content-Type", "text/plain");
                    resp.write_str("Route does not exist", 404);

                };
                error_404(std::move(req), std::move(resp));
            }
        } catch (std::exception &ex) {
            std::cerr << "Exception in " << api_route << std::endl;

            HttpResponse(client_socket).write_str("Server internal error", 500);
        }
       
    }


    // Utils functions 

    RequestType req_type_from_str(const std::string &str) {
        if (str == "GET") {
            return RequestType::GET;
        } else if (str == "POST") {
            return RequestType::POST;
        } else {
            return RequestType::GET;
        }
    }

    std::string process_url_str(const std::string &url) {
        if (url.find("?") == url.npos) {
            return url;
        }

        std::string base_url = url.substr(0, url.find("/") );
     
        std::string query_part = url.substr(url.find("?") + 1);

        do {
            std::string part_query = query_part.substr(0, query_part.find("&")); // Getting key=value out of it

            std::string key = part_query.substr(0, part_query.find("=") + 1); // Getting key
            
            base_url += key + "?";
            if (query_part.find("&") != std::string::npos) { // If there are more query parts add &
                base_url += '&';
            }

            auto end_pos = query_part.find("&");
            query_part = query_part.substr((end_pos == query_part.npos) ? (query_part.size()) : (end_pos + 1));
        } while (!query_part.empty());
        
        return url.substr(0, url.find("?") + 1) + base_url;

    }

    static void sigint_handler(int signal) {
        std::cout << "SIGINT: Closing the server\n";
        delete serv;
        serv = nullptr;
        std::exit(0);
    }
};