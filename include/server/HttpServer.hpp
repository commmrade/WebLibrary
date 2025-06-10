#pragma once

#include <asm-generic/socket.h>
#include <memory>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include<fcntl.h>
#include <unistd.h>
#include "ThreadPool.hpp"

class HttpServer {
private:
   
    int m_serv_socket;
    sockaddr_in m_serv_addr;

    std::unique_ptr<ThreadPool> m_thread_pool;
public: 

    HttpServer(const HttpServer&) = delete;
    HttpServer(HttpServer &&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;
    HttpServer& operator=(HttpServer&&) = delete;

    static HttpServer& instance() {
        static HttpServer serv;
        return serv;
    }

    void listen_start(int port = 8080);
    void stop_server();

private:
    HttpServer();
    ~HttpServer();

    void server_setup(int port);
    void handle_incoming_request(int client_socket);
    std::optional<std::string> read_request(int client_socket);
};