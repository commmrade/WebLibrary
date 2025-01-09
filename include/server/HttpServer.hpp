#pragma once

#include <asm-generic/socket.h>
#include <cerrno>
#include <cstdio>

#include <functional>
#include <memory>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include<csignal>
#include<fcntl.h>
#include <unistd.h>
#include "ThreadPool.hpp"
#include "server/HttpRequest.hpp"
#include "server/HttpResponse.hpp"





using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;
using Filter = std::function<bool(const HttpRequest&)>;

class HttpServer {
private:
   
    int serv_socket;
    sockaddr_in serv_addr;

    //ThreadPool thread_pool;
    std::unique_ptr<ThreadPool> thread_pool;

    std::vector<pollfd> polls_fd;

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
};