#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include<fcntl.h>
#include <unistd.h>
#include <unordered_map>

struct ResponseWriter {
    int sock;
    std::unordered_map<std::string, std::string> headers;
    ResponseWriter(int sock) : sock(sock) {
        
    }
    void write_raw(const std::string &text, int code) {
        std::string response{};
        std::string status_code = std::to_string(code);
        
        response += "HTTP/1.1 " + status_code + " OK\r\n";
    
        //Adding headers part
        for (const auto &[header_name, header_value] : headers) {
            response += header_name + ": " + header_value + "\r\n";
        }

        response += "\r\n"; //Separating headers and answer part

        response += text + "\r\n"; //Adding user text

        ssize_t bytes_sent = send(sock, response.c_str(), response.length(), 0);
        if (bytes_sent < 0) {
            perror("Send error");
        } else {
            //std::cout << "Sent " << bytes_sent << " bytes." << std::endl; // Debug output
        }
    }
    void set_header_raw(std::string name, std::string value) {
        headers[name] = value;
    }
};

struct HttpRequest {
    std::string request;
    std::unordered_map<std::string, std::string> queries;
    std::unordered_map<std::string, std::string> headers;
    
    HttpRequest(const std::string &resp) : request(resp) {
        extract_headers();
        extract_queries();
    }
    void extract_queries() {
        //todo
    }
    void extract_headers() {
        std::string headers_cont = request.substr(request.find("1.1") + 6, request.find("\r\n\r\n") - (request.find("1.1") + 5));

        if (headers_cont.empty()) {
            std::cerr << "Headers not found\n";
            return;
        }

        std::stringstream strm(headers_cont);
        std::string line;
        while (std::getline(strm, line, '\n')) { //Extracting headers one by one
            if (line.find("\r") != line.npos) {
                line.pop_back();
            } 
            
            std::string name = line.substr(0, line.find(":"));
            std::string value = line.substr(name.size() + 2);; 
            
            headers[name] = value;
        }
        
    }
    std::string get_raw() {
        return request;
    }
    std::optional<std::string> get_query(const std::string& query_name) {
        std::string result{};
        
        // Find the start of the query string
        size_t query_start = request.find("?");
        if (query_start == std::string::npos) {
            return std::nullopt; // No query string found
        }
        
        size_t http_start = request.find(" HTTP", query_start);
        if (http_start == std::string::npos) {
            return std::nullopt; // No HTTP found
        }
        
        // Extract the query part
        std::string query_part = request.substr(query_start + 1, http_start - query_start - 1);
        // Find the query parameter
        size_t param_start = query_part.find(query_name + "=");
        if (param_start != std::string::npos) {
            param_start += query_name.length() + 1; // Move to the start of the value
            
            // Find the end of the parameter value
            size_t param_end = query_part.find("&", param_start);
            if (param_end == std::string::npos) {
                param_end = query_part.length(); // No more parameters, take till the end
            }
            // Extract the value
            return result = query_part.substr(param_start, param_end - param_start);
        }
   

        return std::nullopt;
    }
    std::optional<std::string> get_header(const std::string &header_name) {
        auto pos = headers.find(header_name);
        if (pos != headers.end()) { //If header exists
            return pos->second;
        }
        return std::nullopt;
    }
};

enum HandleType {
    POST,
    GET
};

class Route {
private:
    using Callback = std::function<void(HttpRequest&&, ResponseWriter&&)>;
 
    std::unordered_map<std::string, Callback> endpoints_get;
    std::unordered_map<std::string, Callback> endpoints_post;

    int serv_socket;
    sockaddr_in serv_addr;

public: 
    void method_add(HandleType type, std::string &&endpoint_name, Callback foo) {
        switch (type) {
            case GET: {
                endpoints_get[endpoint_name] = foo;
                break;
            }
            default: {
                std::cerr << "EENot implemented yet\n";
                break;
            }
        }
    }
    Route(int port) {
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

       
        
    }

    ~Route() {
        close(serv_socket);
    }
    std::string process_url_str(const std::string &url) {
        std::string base_url = url.substr(0, url.find("?")); //Base url without query part
        std::string purl = url.substr(url.find("?") + 1); //query part of the url
        

        purl = purl.replace(purl.find("=") + 1, purl.find("&") - purl.find("=") - 1, "?"); //Replacing first value with ?
        
        base_url += "?" + purl.substr(0, purl.find("?") + 1);

        if (url.find("&") != url.npos) { //If there are other queries replace them one by one
            for (auto i = 0; i <= std::count(purl.begin(), purl.end(), '&'); i++) {
                purl = purl.substr(purl.find("&") + 1);
                purl = purl.replace(purl.find("=") + 1, purl.find("&") - purl.find("=") - 1, "?");
                base_url += "&" + purl.substr(0, purl.find("?") + 1);   
            }
        }
        return base_url;
    }
    void handle_incoming_request(int client_socket) {
        // const int BUF_LEN = 4096;
        // char buf[BUF_LEN];
        // ssize_t read_bytes = read(client_socket, buf, BUF_LEN); //Writing incoming info to buffer (todo: use string\dynamic allocation)
        // if (read_bytes < 0) {
        //     std::cerr << "ERRor\n";
        //     close(client_socket);
        //     return;
        // }

        std::string call = [client_socket]() {
            std::string result;
            result.reserve(4096); // Reserve some space to avoid multiple allocations
            
            int already_read = 0;
            while (true) {
                result.resize(already_read + 1);
                auto rd_bytes = read(client_socket, result.data() + already_read, 1);
               
                if (rd_bytes == 0) {
                    // End of stream
                    break;
                } else if (rd_bytes < 0) {
                    if (errno == EAGAIN || errno == EWOULDBLOCK) {
              
                        std::cout << "Data ended" << std::endl;
                        break; // or continue, depending on your logic
                    } 
                    std::cerr << "Error reading\n";
                    break;
                }

                already_read += rd_bytes;
            }

            // Resize the string to the actual size read
            result.resize(already_read);
            return result;
        }();

         

        //buf[read_bytes] = '\0'; //Null terminating buffer to avoid problems
        //std::string call = buf; //Turning buf to string

        std::string method = call.substr(0, call.find("/") - 1); //Extracting method from request
        if (method == "GET") {
            std::string api_route = call.substr(call.find("GET") + 4, call.find("HTTP") - (call.find("GET") + 5)); //URL path that was called like /api/route

            if (api_route.find("?") != api_route.npos) {
                std::string base_url = process_url_str(api_route); //Replacing values with ?

                ResponseWriter resp(client_socket);
                HttpRequest req(call);
                if (endpoints_get.find(base_url) != endpoints_get.end()) { //If such api route exist
                    endpoints_get.at(base_url)(std::move(req), std::move(resp));
                } else {
                    endpoints_get["other"](std::move(req), std::move(resp)); //Kinda 404 request analogue
                }
            } else { //If api route is non-query
                ResponseWriter resp(client_socket);
                HttpRequest req(call);
                if (endpoints_get.find(api_route) != endpoints_get.end()) {
                    endpoints_get.at(api_route)(std::move(req), std::move(resp));
                } else {
                    endpoints_get["other"](std::move(req), std::move(resp)); //Kinda 404 request analogue
                }
            }
        } else if (method == "POST") {

        }
        close(client_socket);
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
            int flags = fcntl(client_socket, F_GETFL, 0);
            if (fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1) {
                std::cerr << "Error setting non-blocking mode: " << strerror(errno) << std::endl;
                close(client_socket);
                exit(-1);
            }
            
            std::thread(&Route::handle_incoming_request, this, client_socket).detach(); 
        }
    }
};