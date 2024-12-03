#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <ostream>
#include <shared_mutex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include<csignal>
#include<fcntl.h>
#include "HttpResponse.hpp"
#include "HttpRequest.hpp"
#include <unistd.h>
#include <unordered_map>



enum HandleType {
    POST,
    GET
};

class HttpServer {
private:
    using Callback = std::function<void(HttpRequest&&, HttpResponse&&)>;
 
    std::unordered_map<std::string, Callback> endpoints_get;
    std::unordered_map<std::string, Callback> endpoints_post;

    int serv_socket;
    sockaddr_in serv_addr;

    inline static HttpServer *serv;

    std::shared_mutex mtx;

public: 
    void method_add(HandleType type, std::string &&endpoint_name, Callback foo) {
        switch (type) {
            case GET: {
                endpoints_get[endpoint_name] = foo;
                break;
            }
            case POST: {
                endpoints_post[endpoint_name] = foo;
                break;
            }
            default: {
                std::cerr << "This request type is not implemented yet!\n";
            }
        }
    }
    

    ~HttpServer() {
        close(serv_socket);
    }
    
    static HttpServer& instance(int port = 8080) {
        if (serv == nullptr) {
            serv = new HttpServer(port);
        }
        return *serv;
    }

    void listen_start() {
        if (endpoints_get.empty() && endpoints_post.empty()) {
            throw std::runtime_error("Endpoints methods are not set");
        }

         while (true) {
            int client_socket = accept(serv_socket, nullptr, nullptr);
            if (client_socket < 0) {
                perror("Client socket");
                exit(-1);
            }
            
          
            std::thread(&HttpServer::handle_incoming_request, this, client_socket).detach(); 
        }
    }
private:
    HttpServer(int port) {

        endpoints_get["other"] = [](auto &&req, auto &&resp) {
            resp.set_header_raw("Content-Type", "text/plain");
            resp.write_str("Route does not exist", 404);
        };


        serv_socket = socket(AF_INET, SOCK_STREAM, 0); //Creating a socket that can be connected to
        if (serv_socket < 0) {
            perror("Serv socket");
            exit(-1);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port); //Settings app addres info
        serv_addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(serv_socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { //Binding socket
            std::cerr << "Error binding...\n";
            perror("Binding");
            exit(-1);
        } 


        if (listen(serv_socket, 999) < 0) { //Listening for incoming requests
            perror("Listening");
            exit(-1);
        }
        

       
        signal(SIGINT, &HttpServer::sigint_handler);
    }

    

    void handle_incoming_request(int client_socket) {
        fcntl(client_socket, F_SETFL, O_NONBLOCK);
        std::string call;
        call.resize(4096);

        size_t already_read{0};
        int rd_bytes;
        while ((rd_bytes = read(client_socket, call.data() + already_read, 4096)) > 0) {
            already_read += rd_bytes;   

            if (already_read >= call.size()) {
                call.resize(call.size() * 2);
            }
        }
       
        call.resize(already_read);
        call.shrink_to_fit();


        process_to_endpoint(client_socket, call);
        
        close(client_socket);
    }


    void process_to_endpoint(int client_socket, const std::string &call) {
        std::shared_lock lock{mtx};

        std::string method = call.substr(0, call.find("/") - 1); //Extracting method from request
        if (method == "GET") {
            std::string api_route = call.substr(call.find(" ") + 1, call.find("HTTP") - (call.find(" ") + 1)); //URL path that was called like /api/HttpServer
            std::string base_url = process_url_str(api_route); //Replacing values with ?

            HttpResponse resp(client_socket);
            HttpRequest req(call);

            if (endpoints_get.find(base_url) != endpoints_get.end()) { //If such api HttpServer exist
                endpoints_get.at(base_url)(std::move(req), std::move(resp));
            } else {
                endpoints_get["other"](std::move(req), std::move(resp)); //Kinda 404 request analogue
            }
        } else if (method == "POST") {
            std::string api_route = call.substr(call.find(" ") + 1, call.find("HTTP") - (call.find(" ") + 1)); //URL path that was called like /api/HttpServer
            std::string base_url = process_url_str(api_route);

            HttpResponse resp(client_socket);
            HttpRequest req(call);

            if (endpoints_post.find(base_url) != endpoints_post.end()) { //If such api HttpServer exist
                endpoints_post.at(base_url)(std::move(req), std::move(resp));
            } else {
                endpoints_post["other"](std::move(req), std::move(resp)); //Kinda 404 request analogue
            }
        }
    }


    std::string process_url_str(const std::string &url) {
        std::string base_url = url.substr(0, url.find("?")); //Base url without query part
        std::string queries = url.substr(url.find("?") + 1); //query part of the url
        

        queries = queries.replace(queries.find("=") + 1, queries.find("&") - queries.find("=") - 1, "?"); //Replacing first value with ?
        
        base_url += "?" + queries.substr(0, queries.find("?") + 1);

        if (url.find("&") != url.npos) { //If there are other queries replace them one by one
            for (auto i = 0; i <= std::count(queries.begin(), queries.end(), '&'); i++) {
                queries = queries.substr(queries.find("&") + 1);
                queries = queries.replace(queries.find("=") + 1, queries.find("&") - queries.find("=") - 1, "?");
                base_url += "&" + queries.substr(0, queries.find("?") + 1);   
            }
        }
        return base_url;
    }

    void stop_server() {
        close(serv_socket);
        std::cout << "socket closed?\n";
    }
    static void sigint_handler(int signal) {
        std::cout << "SIGINT: Closing the server\n";
        delete serv;
        std::exit(0);
    }

    
};