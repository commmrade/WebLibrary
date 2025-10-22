// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once

#include <asm-generic/socket.h>
#include <memory>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>
#include "ThreadPool.hpp"
namespace weblib {





class HttpServer
{
struct Client {
    int fd;
    std::string raw_http;
    int  content_length{-1};
    int  body_bytes_rd{0};
    bool is_in_body{false};
    size_t header_end_pos = 0;

    enum class State {
        READ_MORE, // EWOULDBLOCK || EAGAIN
        CONNECTION_ABORTED, // EOF
        END_OF_CONNECTION, // Normal end
        CONNECTION_ERROR // Errnous behaviour
    };
};

  private:
    int         m_listen_socket;
    sockaddr_in m_listen_addr;
    bool is_running{};

    std::unique_ptr<ThreadPool<>> m_thread_pool;

    std::unordered_map<int, Client> m_active_clients;
  public:
    HttpServer(const HttpServer &)            = delete;
    HttpServer(HttpServer &&)                 = delete;
    HttpServer &operator=(const HttpServer &) = delete;
    HttpServer &operator=(HttpServer &&)      = delete;

    static auto instance() -> HttpServer &
    {
        static HttpServer serv;
        return serv;
    }

    void listen_start(int port = 8080);
    void stop_server();

  private:
    HttpServer();
    ~HttpServer();
  private:
    void event_loop();
    void server_setup(int port);
    void handle_incoming_request(int client_socket);
    // auto read_request(int client_socket) -> std::optional<std::string>;
    auto read_request(Client& client) -> Client::State;
};

} // namespace weblib