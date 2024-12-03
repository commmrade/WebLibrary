#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <server/Endpoint.hpp>
#include<print>

void reg(HttpRequest&& req, HttpResponse&& resp) {
    std::cout << "reg\n";
    auto login = req.get_query("name");
    auto password = req.get_query("password");
    
    if (login->empty() || password->empty()) {
        std::println("Empty");
        resp.set_header_raw("Content-Type", "text/plain");
        resp.write_str("ne works", 400);
        return;
    }

    std::cout << req.get_query("password").value_or("NULL") << std::endl;
    resp.write_str("Works", 200);
}



int main() {
    auto &app = HttpServer::instance();
    //app.method_add(GET, "/zov?name=?&password=?", reg);
    app.method_add(POST, "/zov?name=?&password=?", reg);

    app.listen_start();
    
    return 0;
}





