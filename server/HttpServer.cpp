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
    int flags = fcntl(m_listen_socket, F_GETFL, 0);
    if (flags < 0 || fcntl(m_listen_socket, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        throw socket_flags_error{}; 
    }

    int reuse  = 1;
    int result = setsockopt(m_listen_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
    if (result < 0)
    {
        debug::log_error("Could not set flags: ", strerror(errno));
        throw socket_flags_error{};
    }

    m_listen_addr.sin_family      = AF_INET;
    m_listen_addr.sin_port        = htons(port);
    m_listen_addr.sin_addr.s_addr = INADDR_ANY;

    debug::log_info("Binding socket");
    if (bind(m_listen_socket, (sockaddr *)&m_listen_addr, sizeof(m_listen_addr)) < 0)
    { // Binding socket
        debug::log_error("Binding socket error");
        throw socket_bind_error{};
    }
}

auto HttpServer::read_request(int client_socket) -> std::optional<std::string>
{
    std::string raw_http;

    int  content_length{-1};
    int  body_bytes_rd{0};
    bool is_in_body{false};

    size_t header_end_pos = 0;
    while (true)
    {
        std::array<char, 4096> buf{};
        ssize_t rd_bytes = read(client_socket, buf.data(), buf.size());
        // if (rd_bytes == 0)
        // {
        //     debug::log_info("Client has disconnected");
        //     return std::nullopt;
        // }
        


        if (rd_bytes > 0)
        {
            raw_http.append(buf.data(),
                                  rd_bytes);
            if (!is_in_body &&
                (header_end_pos = raw_http.find(HttpConsts::CRNLCRNL)) != std::string::npos)
            {
                auto headers_start = raw_http.find(HttpConsts::CRNL) + HttpConsts::CRNL.size();
                auto header_string =
                    raw_http.substr(headers_start, header_end_pos - headers_start);
                HttpHeaders headers{std::move(header_string)};

                try
                {
                    content_length = std::stoi(
                        headers.get_header("Content-Length")
                            .value_or(
                                "0"));
                }
                catch (const std::invalid_argument &ex)
                {
                    debug::log_warn("Could not parse content-length: ", ex.what());
                    content_length = 0;
                }

                if (content_length == 0) { // No body request
                    return raw_http;
                }

                is_in_body = true;
            }
            else if (is_in_body)
            {
                body_bytes_rd = std::distance(
                    raw_http.begin() + header_end_pos + HttpConsts::CRNLCRNL.size(),
                    raw_http.end()); // Cheap because its a random access iterator
                if (body_bytes_rd >= content_length)
                {
                    return raw_http;
                }
            }
        }
        else if (rd_bytes < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
        {
            continue;
        }
        else
        {
            debug::log_error("Fatal error when reading");
            throw reading_socket_error{};
        }
    }
}

void HttpServer::handle_incoming_request(int client_socket)
{
    debug::log_info("Reading user request");
    try
    {
        while (true)
        {
            auto raw_http = read_request(client_socket);
            if (!raw_http)
            {
                break;
            }

            HttpRouter::instance().process_request(client_socket, raw_http.value());
        }
        close(client_socket);
    }
    catch (const std::exception &ex)
    {
        close(client_socket);
    }
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
    while (true)
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
                { // If server socket got something
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
                    poll_fds.push_back({client_socket, POLLIN, 0}); // Add new user
                }
                else
                {
                    int sock = cur_fd.fd;

                    m_thread_pool->enqueue_detach(&HttpServer::handle_incoming_request, this,
                                                  sock);

                    poll_fds.erase(poll_fds.begin() + i);
                    i--;
                }
            }
        }
    }
}
void HttpServer::stop_server() { close(m_listen_socket); }

} // namespace weblib