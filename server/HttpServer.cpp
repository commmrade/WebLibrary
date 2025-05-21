#include <cerrno>
#include <server/HttpServer.hpp>
#include <server/HttpRouter.hpp>
#include <stdexcept>
#include <sys/poll.h>
#include <sys/types.h>
#include "debug.hpp"


HttpServer::HttpServer() {
    thread_pool = std::make_unique<ThreadPool>();
}
HttpServer::~HttpServer() {
    close(serv_socket);
}


void HttpServer::server_setup(int port) {
    debug::log_info("Setting up server");


    debug::log_info("Creating a socket");
    serv_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // Creating a socket that can be connected to
    if (serv_socket < 0) {
        debug::log_error("Socket error");
        throw std::runtime_error("");
    }   
    int flags = fcntl(serv_socket, F_GETFL, 0);
    fcntl(serv_socket, F_SETFL, flags | O_NONBLOCK);

    int reuse = 1;
    int result = setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse));
    if ( result < 0 ) {
        perror("ERROR SO_REUSEADDR:");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); // Settings app addres info
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    debug::log_info("Binding socket");
    if (bind(serv_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { // Binding socket
        debug::log_error("Binding socket error");
        throw std::runtime_error("");
    } 

    
    
}

void HttpServer::handle_incoming_request(int client_socket) {
    debug::log_info("Reading user request");

    fcntl(client_socket, F_SETFL, O_NONBLOCK); // Setting non-blocking mode for better handling of requests
    std::string request_string; 
    request_string.resize(4096);
    
    size_t already_read{0};
    while (true) {
        ssize_t rd_bytes = read(client_socket, request_string.data() + already_read, 4096);
        if (rd_bytes == 0) { // CLient disconnected
            debug::log_info("Client has disconnected");
            break;
        }
        if (rd_bytes > 0) {
            already_read += rd_bytes;
            if ((int)request_string.size() - already_read < 4096) {
                request_string.resize(request_string.size() + 4096);
            }
            if (rd_bytes < 4096) { // We read the full request TODO: If its not GET parse headers find content-length and use it to read
                request_string.resize(already_read);
                HttpRouter::instance().process_endpoint(client_socket, request_string); // Read/Writes are kinda thread-safe but at the same time they arent so idk leave it like that
                request_string.resize(4096); // Old content will be just overwritten
                already_read = 0;
            }
        } else if (rd_bytes < 0 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
            pollfd client;
            client.fd = client_socket;
            client.events = POLLIN;
            int poll_result = poll(&client, 1, 5000);
            if (poll_result <= 0) { // Probably means connection is kinda lost
                debug::log_error("Connection is lost");
                break;
            }
        } else {
            debug::log_error("Fatal error when reading");
            throw std::runtime_error("Fatal error reading");
        }

    }
    close(client_socket);
    client_socket = -1;
}


void HttpServer::listen_start(int port) {
    server_setup(port);


    debug::log_info("Starting listening for incoming requests");
    if (listen(serv_socket, SOMAXCONN) < 0) { // Listening for incoming requests
        debug::log_error("Listening error");
        throw std::runtime_error("");
    }


    polls_fd.push_back({serv_socket, POLLIN, 0}); // Setting server socket
    while (true) {
        int poll_result = poll(polls_fd.data(), polls_fd.size(), -1); // Polling for inf time
        if (poll_result < 0) {
            debug::log_error("Polling error"); // Critical error
            throw std::runtime_error("");
        }
        
        for (size_t i = 0; i < polls_fd.size(); i++) {
            if (polls_fd[i].revents & POLLIN) {
                if (polls_fd[i].fd == serv_socket) { // If server socket got something

                    int client_socket = accept(serv_socket, nullptr, nullptr);
                    if (client_socket < 0) {
                        debug::log_error("Accepting user error");
                        continue; // Moving on
                    }

                    polls_fd.push_back({client_socket, POLLIN, 0}); // Add new user
                } else {
                    int client_socket = polls_fd[i].fd;

                    thread_pool->add_job([this, client_socket](){ handle_incoming_request(client_socket); }); // Proccess user and remove from poll
                    polls_fd.erase(polls_fd.begin() + i);
                    i--;
                }

            }
        }
        
    }
}
void HttpServer::stop_server() {
    close(serv_socket);
}