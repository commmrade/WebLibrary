#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <unordered_map>
#include "Endpoint.hpp"
#include<print>
#include<signal.h>
const int PORT = 0;




void reg(HttpRequest&& req, ResponseWriter&& resp) {
    auto login = req.get_query("name");
    auto password = req.get_query("password");
    std::cout << req.get_raw() << std::endl;
    if (login->empty() || password->empty()) {
        std::println("Empty");
        resp.set_header_raw("Content-Type", "text/plain");
        resp.write_raw("ne works", 400);
        return;
    }

    resp.set_header_raw("Content-Type", "text/plain");
    resp.write_raw("Works", 200);
}
void other(HttpRequest&& req, ResponseWriter&& resp) {
    
    std::println("404 request");

    resp.set_header_raw("Content-Type", "text/plain");
    resp.write_raw("Page does not exist", 200);
}


int main() {

    Route route(8080);

    route.method_add(GET, "/register?name=?&password=?", reg);
    route.method_add(GET, "other", other);
    
    route.listen_start();

    return 0;
}
