
#include <sys/socket.h>
#include<unordered_map>
#include<string>


struct HttpResponse {
    int sock;
    std::unordered_map<std::string, std::string> headers;
    HttpResponse(int sock) : sock(sock) {
        headers["Content-Type"] = "text/plain";
    }
    void write_str(const std::string &text, int code) {
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