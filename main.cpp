#include <cstdio>
#include <functional>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <requests/request.hpp>
#include<print>




int main() {
    get_client joke("https://official-joke-api.appspot.com/jokes/random");
    
    response resp = Request::execute(joke);
    
    std::cout << resp.text() << std::endl;
    
    return 0;
}





// void reg(HttpRequest&& req, ResponseWriter&& resp) {
//     auto login = req.get_query("name");
//     auto password = req.get_query("password");
//     std::cout << req.get_raw() << std::endl;
//     if (login->empty() || password->empty()) {
//         std::println("Empty");
//         resp.set_header_raw("Content-Type", "text/plain");
//         resp.write_raw("ne works", 400);
//         return;
//     }

//     resp.set_header_raw("Content-Type", "text/plain");
//     resp.write_raw("Works", 200);
// }
// void other(HttpRequest&& req, ResponseWriter&& resp) {
    
//     std::println("404 request");

//     resp.set_header_raw("Content-Type", "text/plain");
//     resp.write_raw("Page does not exist", 200);
// }
