#include "server/HttpResController.hpp"
#include <server/HttpServer.hpp>
#include <debug.hpp>

static HttpServer& app();

static void sigint_handler([[maybe_unused]] int signal) {
    debug::log_info("sigint: Closing the server");
    app().stop_server();
    std::exit(0);
}

static HttpServer& app() {
    signal(SIGINT, sigint_handler);
    return HttpServer::instance();
}
