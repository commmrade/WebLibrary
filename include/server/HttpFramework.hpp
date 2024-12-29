#include "server/HttpResController.hpp"
#include <server/HttpServer.hpp>


static HttpServer& app();

static void sigint_handler([[maybe_unused]] int signal) {
    std::cout << "SIGINT: Closing the server\n";
    app().stop_server();
    std::exit(0);
}

static HttpServer& app() {
    signal(SIGINT, sigint_handler);
    return HttpServer::instance();
}
