#include <server/HttpServer.hpp>
#include <debug.hpp>
#include <signal.h>

static HttpServer& app();

static void sigint_handler([[maybe_unused]] int signal) {
    debug::log_info("SIGINT: Closing the server");
    app().stop_server();
    std::exit(0);
}

static HttpServer& app() {
    signal(SIGINT, sigint_handler);
    return HttpServer::instance();
}
