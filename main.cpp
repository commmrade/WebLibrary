#include "server/HttpServer.hpp"
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
#include <server/CustomFilter.hpp>
#include <server/CustomFilter2.hpp>
#include<print>




int main() {

    MyController a{};

    

   
    HttpServer::instance().listen_start();
    
    
    return 0;
}





