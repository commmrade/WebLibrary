// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#include "weblib/server/HttpServer.hpp"
#include <debug.hpp>
#include <signal.h>

static auto app() -> HttpServer &;

static void sigint_handler([[maybe_unused]] int signal)
{
    debug::log_info("SIGINT: Closing the server");
    app().stop_server();
    std::exit(0);
}

[[nodiscard]]
static auto app() -> HttpServer &
{
    signal(SIGINT, sigint_handler);
    return HttpServer::instance();
}
