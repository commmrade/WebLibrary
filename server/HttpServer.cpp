#include <server/HttpServer.hpp>

HttpServer::HttpServer() {
    HttpResController resController; // Setting up resource controller
    thread_pool.create();
}
HttpServer::~HttpServer() {
    thread_pool.stop();
    close(serv_socket);
}


void HttpServer::server_setup(int port) {
    debug::log_info("Setting up server");


    debug::log_info("Creating a socket");
    serv_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); // Creating a socket that can be connected to
    if (serv_socket < 0) {
        debug::log_error("Socket error");
        std::abort();
    }   

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); // Settings app addres info
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    debug::log_info("Binding socket");
    if (bind(serv_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { // Binding socket
        debug::log_error("Binding socket error");
        std::abort();
    } 

    
    
}

void HttpServer::handle_incoming_request(int client_socket) {
    debug::log_info("Reding user request");

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
            debug::log_error("Error reading");
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


void HttpServer::listen_start(int port) {
    server_setup(port);


    debug::log_info("Starting listening for incoming requests");
    if (listen(serv_socket, SOMAXCONN) < 0) { // Listening for incoming requests
        debug::log_error("Listening error");
        std::abort();
    }


    polls_fd.push_back({serv_socket, POLLIN, 0}); // Setting server socket
    while (true) {
        int poll_result = poll(polls_fd.data(), polls_fd.size(), -1); // Polling for inf time because -1
        if (poll_result < 0) {
            debug::log_error("Polling error");
            std::abort();
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

                    thread_pool.add_job([this](int client_socket){ handle_incoming_request(client_socket); }, client_socket); // Proccess user and remove from poll
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