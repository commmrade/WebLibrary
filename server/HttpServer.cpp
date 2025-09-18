#include "weblib/server/HttpServer.hpp"
#include <cerrno>
#include <cstring>
#include <exception>
#include <iterator>
#include <optional>
#include "weblib/server/HttpRouter.hpp"
#include <stdexcept>
#include <sys/poll.h>
#include <sys/types.h>
#include "weblib/debug.hpp"
#include "weblib/server/HttpRequest.hpp"
#include <print>
#include <system_error>
#include <array>

HttpServer::HttpServer() {
    m_thread_pool = std::make_unique<ThreadPool<>>();
}
HttpServer::~HttpServer() {
    close(m_serv_socket);
}

static constexpr std::string HEADERS_END = "\r\n\r\n";

void HttpServer::server_setup(int port) {
    debug::log_info("Setting up server");

    debug::log_info("Creating a socket");
    m_serv_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // Creating a socket that can be connected to
    if (m_serv_socket < 0) {
        debug::log_error("Socket error");
        throw std::system_error{std::error_code{}, "Could not create a socket"}; // Since socket() interfaces with the OS, throw system_error 
    }   
    int flags = fcntl(m_serv_socket, F_GETFL, 0);
    if (flags < 0 || fcntl(m_serv_socket, F_SETFL, flags | O_NONBLOCK) < 0) { // Setting non-blocking mode for better handling of requests
        throw std::system_error(std::error_code{}, "Could not set flags for client socket");
    }

    int reuse = 1;
    int result = setsockopt(m_serv_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
    if (result < 0) {
        debug::log_error("Could not set flags: ", strerror(errno));
        throw std::system_error{std::error_code{}, "Could not set flags"};
    }

    m_serv_addr.sin_family = AF_INET;
    m_serv_addr.sin_port = htons(port); // Settings app addres info
    m_serv_addr.sin_addr.s_addr = INADDR_ANY;

    debug::log_info("Binding socket");
    if (bind(m_serv_socket, (sockaddr*)&m_serv_addr, sizeof(m_serv_addr)) < 0) { // Binding socket
        debug::log_error("Binding socket error");
        throw std::system_error(std::error_code{}, "Could not bind the server socket");
    } 
}

auto HttpServer::read_request(int client_socket) -> std::optional<std::string> {
    std::string request_string; 

    int content_length{-1};
    int current_body_read{0};
    bool in_body{false};

    size_t header_end_pos = 0;
    while (true) {
        std::array<char, 4096> buffer{};
        ssize_t rd_bytes = read(client_socket, buffer.data(), 4096);
        if (rd_bytes == 0) { // CLient disconnected
            debug::log_info("Client has disconnected");
            return std::nullopt;
        }
        if (rd_bytes > 0) { // If something to read...
            request_string.append(buffer.data(), rd_bytes); // This way to need for some resizing logic
            // Body parsing
            if (!in_body && (header_end_pos = request_string.find(HEADERS_END)) != std::string::npos) { // Store header_end_pos, so no need to calculate it in b_in_body branch
                // std::println("1");
                HttpRequest req{request_string, ""}; // TODO: Just parse headers, this will be faster
                // std::println("2");
                try {
                    content_length = std::stoi(req.get_header("Content-Length").value_or("0")); // If no header set length to 0 (For example in GET requests)
                } catch (const std::invalid_argument& ex) {
                    debug::log_warn("Could not parse content-length: ", ex.what());
                    content_length = 0; // We don't care what exception it is just set c-l to 0
                }
                in_body = true;
            } 
            if (in_body) { // If parsing the body
                current_body_read = std::distance(request_string.begin() + header_end_pos + 4, request_string.end()); // Cheap because its a random access iterator
                if (current_body_read >= content_length) { // Finished reading body, if body is empty it will still be true, since default content-length value is 0 and current_body_read is 0 by default
                    return request_string;
                }
            }
        } else if (rd_bytes < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            pollfd client{};
            client.fd = client_socket;
            client.events = POLLIN;
            int poll_result = poll(&client, 1, 5000);
            if (poll_result <= 0) { // Probably means connection is kinda lost
                debug::log_error("Connection is lost");
                return std::nullopt;
            }
        } else {
            debug::log_error("Fatal error when reading");
            throw std::runtime_error("Fatal error reading");
        }
    }
}

void HttpServer::handle_incoming_request(int client_socket) {
    debug::log_info("Reading user request");
    try {
        while (true) {
            auto request_str = read_request(client_socket);
            if (!request_str) {
                break;
            }
            
            HttpRouter::instance().process_request(client_socket, request_str.value());
        }
        close(client_socket);
    } catch (const std::exception& ex) {
        close(client_socket);
    }
    
}


void HttpServer::listen_start(int port) {
    server_setup(port);

    debug::log_info("Starting listening for incoming requests");
    if (listen(m_serv_socket, SOMAXCONN) < 0) { // Listening for incoming requests
        debug::log_error("Listening error");
        throw std::runtime_error("");
    }
    std::vector<pollfd> polls_fd;
    polls_fd.push_back({m_serv_socket, POLLIN, 0}); // Setting server socket
    while (true) {
        int poll_result = poll(polls_fd.data(), polls_fd.size(), -1); // Polling for inf time
        
        if (poll_result < 0) {
            debug::log_error("Polling error"); // Critical error
            throw std::runtime_error("");
        }
        for (size_t i = 0; i < polls_fd.size(); i++) {
            if ((polls_fd[i].revents & POLLIN) == 1) {
                if (polls_fd[i].fd == m_serv_socket) { // If server socket got something
                    int client_socket = accept(m_serv_socket, nullptr, nullptr);
                    if (client_socket < 0) {
                        debug::log_error("Accepting user error");
                        continue; // Moving on
                    }
                    int flags = fcntl(client_socket, F_GETFL, 0);
                    if (flags < 0 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) < 0) { // Setting non-blocking mode for better handling of requests
                        throw std::runtime_error("Could not set flags for client socket");
                    }
                    polls_fd.push_back({client_socket, POLLIN, 0}); // Add new user
                } else {
                    int client_socket = polls_fd[i].fd;

                    m_thread_pool->enqueue_detach(&HttpServer::handle_incoming_request, this, client_socket);
                    
                    polls_fd.erase(polls_fd.begin() + i);
                    i--;
                }
            }
        }
        
    }
}
void HttpServer::stop_server() {
    close(m_serv_socket);
}