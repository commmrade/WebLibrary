#pragma once

#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdio>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include<csignal>
#include<fcntl.h>
#include <unistd.h>
#include "ThreadPool.hpp"
#include "HttpRouter.hpp"
#include "server/HttpResController.hpp"




using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;
using Filter = std::function<bool(const HttpRequest&)>;

class HttpServer {
private:
   
    int serv_socket;
    sockaddr_in serv_addr;

    ThreadPool thread_pool;

    std::vector<pollfd> polls_fd;

public: 

    HttpServer(const HttpServer&) = delete;
    HttpServer(HttpServer &&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer& operator=(HttpServer&&) = delete;


    ~HttpServer() {
        thread_pool.stop();
        close(serv_socket);
    }
    
    static HttpServer& instance(int port = 8080) {
        static HttpServer serv{port};

        return serv;
    }

 
    void listen_start() {
        polls_fd.push_back({serv_socket, POLLIN, 0}); // Setting server socket
        while (true) {
            
            int poll_result = poll(polls_fd.data(), polls_fd.size(), -1); // Polling for inf time because -1
            if (poll_result < 0) {
                perror("Polling error");
                exit(-1);
            }
            
            for (size_t i = 0; i < polls_fd.size(); i++) {
                if (polls_fd[i].revents & POLLIN) {

                    if (polls_fd[i].fd == serv_socket) { // If server socket got something

                        int client_socket = accept(serv_socket, nullptr, nullptr);
                        if (client_socket < 0) {
                            perror("Client error");
                            exit(-1);
                        }

                        polls_fd.push_back({client_socket, POLLIN, 0}); // Add new user
                    } else {
                        int client_socket = polls_fd[i].fd;

                        thread_pool.add_job([this](int client_socket){ handle_incoming_request(client_socket); }, client_socket); // Proccess user and remove from poll
                        polls_fd.erase(polls_fd.begin() + i);
                        i--;
                    }

                }
            }
            
        }
    }
    void stop_server() {
        close(serv_socket);
    }

private:
    HttpServer(int port) {
        server_setup(port);

        HttpResController resController; // Setting up resource controller

        thread_pool.create();
   
    }


    void server_setup(int port) {
        serv_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // Creating a socket that can be connected to
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
            perror("Binding error");
            exit(-1);
        } 


        if (listen(serv_socket, 999) < 0) { // Listening for incoming requests
            perror("Listening error");
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
           
            if (rd_bytes > 0) { // Normal reading
                already_read += rd_bytes;   
               

                if (already_read >= call.size()) {
                    call.resize(call.size() * 2);
                }

            } else if (rd_bytes == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
                // Try again
        
            } else if (rd_bytes == -1) {
                std::cerr << "Weird error when reading\n";
                perror("Error reading");
                break; // unknown error when reading
            }

            if (already_read > 0 && already_read <= 4096) {
                break; // Got all data
            }

            
        }
        
        call.resize(already_read);
        call.shrink_to_fit();

       
        HttpRouter::instance().process_endpoint(client_socket, call);

        close(client_socket);
    }

    
    

    
};