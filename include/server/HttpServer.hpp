#pragma once

#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdio>
#include <functional>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include<csignal>
#include<fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include "hash.hpp"
#include "ThreadPool.hpp"
#include "HttpRouter.hpp"




using Handler = std::function<void(HttpRequest&&, HttpResponse&&)>;
using Filter = std::function<bool(HttpRequest&&)>;

class HttpServer {
private:
   
    int serv_socket;
    sockaddr_in serv_addr;



    
    bool is_running{false};

    ThreadPool thread_pool;

public: 

    ~HttpServer() {
        thread_pool.stop();
        close(serv_socket);
    }
    
    static HttpServer& instance(int port = 8080) {
        static HttpServer serv{port};

        return serv;
    }

    bool is_ran() const { return is_running; }

    void listen_start() {
        is_running = true;

       

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
       
        fcntl(client_socket, F_SETFL, O_NONBLOCK); // Setting non-blocking mode for better handling of requests

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

       
        HttpRouter::instance().process_endpoint(client_socket, call);

        close(client_socket);
    }

    
    void stop_server() {
        close(serv_socket);
    }

    static void sigint_handler(int signal) {
        std::cout << "SIGINT: Closing the server\n";
        instance().stop_server();
        std::exit(0);
    }
};