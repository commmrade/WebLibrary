#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <server/HttpServer.hpp>
#include<print>

void reg(HttpRequest&& req, HttpResponse&& resp) {
    
    auto login = req.get_query("name");
    auto password = req.get_query("password");
    
    if (login->empty() || password->empty()) {
        std::println("Empty");
        resp.set_header_raw("Content-Type", "text/plain");
        resp.write_str("ne works", 400);
        return;
    }

    std::cout << "start\n";
    std::ifstream file("index.html");
    std::stringstream ss;
    ss << file.rdbuf();
    std::cout << "end\n";
    resp.set_header_raw("Content-Type", "text/html");
    resp.write_str(ss.str(), 200);
}



int main() {
    auto &app = HttpServer::instance();
    //app.method_add(GET, "/zov?name=?&password=?", reg);

    
    app.method_add(RequestType::GET, "/zov?name=?&password=?", reg);

    app.listen_start();
    
    return 0;
}





