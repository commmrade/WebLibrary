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
#include <server/CustomController.hpp>

#include<print>




int main() {
    auto &app = HttpServer::instance();
    //app.method_add(GET, "/zov?name=?&password=?", reg);
    MyController::init_path_routing();
    
    
    app.listen_start();
    
    return 0;
}





