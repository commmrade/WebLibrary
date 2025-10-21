#include "weblib/server/HttpServer.hpp"
#include <cerrno>
#include <cstring>
#include <exception>
#include <iterator>
#include <optional>
#include "weblib/exceptions.hpp"
#include "weblib/server/HttpRouter.hpp"
#include <stdexcept>
#include <sys/poll.h>
#include <sys/types.h>
#include "weblib/debug.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <array>
#include "weblib/consts.hpp"


namespace weblib {
HttpServer::HttpServer() { m_thread_pool = std::make_unique<ThreadPool<>>(); }
HttpServer::~HttpServer() { close(m_listen_socket); }

void HttpServer::server_setup(int port)
{
    debug::log_info("Setting up server");

    debug::log_info("Creating a socket");
    m_listen_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK,
                           0);
    if (m_listen_socket < 0)
    {
        debug::log_error("Socket error");
        throw socket_creation_error{};
    }
  
    int reuse  = 1;
    int result = setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
    if (result < 0)
    {
        debug::log_error("Could not set flags: ", strerror(errno));
        throw socket_flags_error{};
    }

    m_listen_addr.sin_family      = AF_INET; // TOOD: Support ipv6?
    m_listen_addr.sin_port        = htons(port);
    m_listen_addr.sin_addr.s_addr = INADDR_ANY;

    debug::log_info("Binding socket");
    result = bind(m_listen_socket, (sockaddr *)&m_listen_addr, sizeof(m_listen_addr));
    if (result < 0)
    { // Binding socket
        debug::log_error("Binding socket error");
        throw socket_bind_error{};
    }
    is_running = true;
}

auto HttpServer::read_request(Client& client) -> Client::State {
    std::array<char, 4096> buf{};
    ssize_t rd_bytes = read(client.fd, buf.data(), buf.size());
    if (rd_bytes == 0) {
        debug::log_info("Client has disconnected");
        return Client::State::CONNECTION_ABORTED;
    }

    if (rd_bytes < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
        return Client::State::READ_MORE;
    } else if (rd_bytes < 0) {
        return Client::State::CONNECTION_ERROR;
    }

    client.raw_http.append(buf.data(), rd_bytes);
    if (!client.is_in_body && (client.header_end_pos = client.raw_http.find(HttpConsts::CRNLCRNL)) != std::string::npos) {
        auto headers_start = client.raw_http.find(HttpConsts::CRNL) + HttpConsts::CRNL.size();
        auto header_string = client.raw_http.substr(headers_start, client.header_end_pos - headers_start);
        HttpHeaders headers{std::move(header_string)};

        try
        {
            client.content_length = std::stoi(
                headers.get_header("Content-Length")
                    .value_or(
                        "0"));
        }
        catch (const std::invalid_argument &ex)
        {
            debug::log_warn("Could not parse content-length: ", ex.what());
            client.content_length = 0;
        }

        if (client.content_length == 0) {
            return Client::State::END_OF_CONNECTION;
        }

        client.is_in_body = true;
    } else if (client.is_in_body) {
        client.body_bytes_rd = std::distance(
            client.raw_http.begin() + client.header_end_pos + HttpConsts::CRNLCRNL.size(),
            client.raw_http.end()); // Cheap because its a random access iterator
        if (client.body_bytes_rd >= client.content_length)
        {
            return Client::State::END_OF_CONNECTION;
        }
    }
    return Client::State::READ_MORE;
}


void HttpServer::handle_incoming_request(int client_socket) {
    try {
        Client& client = m_active_clients[client_socket];
        HttpRouter::instance().process_request(client_socket, client.raw_http);
    } catch (const std::exception& ex) {
        debug::log_error("Could not handle client, because ", ex.what());
    }
    close(client_socket);
}

void HttpServer::listen_start(int port)
{
    server_setup(port);

    debug::log_info("Starting listening for incoming requests");
    if (listen(m_listen_socket, SOMAXCONN) < 0)
    {
        debug::log_error("Listening error");
        throw std::runtime_error("");
    }
    std::vector<pollfd> poll_fds;
    poll_fds.push_back({m_listen_socket, POLLIN, 0}); // Setting server socket
    while (is_running)
    {
        int poll_result = poll(poll_fds.data(), poll_fds.size(), -1); // Polling for inf time

        if (poll_result < 0)
        {
            debug::log_error("Polling error"); // Critical error
            throw std::runtime_error("");
        }
        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            auto& cur_fd = poll_fds[i]; 
            if (cur_fd.revents & POLLIN)
            {
                if (cur_fd.fd == m_listen_socket)
                {
                    int client_socket = accept(m_listen_socket, nullptr, nullptr);
                    if (client_socket < 0)
                    {
                        debug::log_error("Accepting user error");
                        continue;
                    }
                    int flags = fcntl(client_socket, F_GETFL, 0);
                    if (flags < 0 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) < 0)
                    {
                        throw socket_flags_error{};
                    }
                    poll_fds.push_back({client_socket, POLLIN, 0});
                    m_active_clients.emplace(client_socket, Client{client_socket, {}, -1, 0, false, 0});
                }
                else
                {
                    Client& client = m_active_clients[cur_fd.fd];
                    Client::State r = read_request(client);
                    switch (r) {
                        case Client::State::READ_MORE: {
                            continue;
                        }
                        case Client::State::END_OF_CONNECTION: {
                            m_thread_pool->enqueue_detach(&HttpServer::handle_incoming_request, this,
                                                  client.fd);
                            break;
                        }
                        case Client::State::CONNECTION_ABORTED:
                        case Client::State::CONNECTION_ERROR: {
                            poll_fds.erase(poll_fds.begin() + i);
                            m_active_clients.erase(client.fd);
                            i--;
                        }
                    }
                }
            }
        }
    }
}


void HttpServer::stop_server() { close(m_listen_socket); is_running = false; }

} // namespace weblib