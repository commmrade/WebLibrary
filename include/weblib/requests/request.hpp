// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once 
#include <cerrno>
#include <exception>
#include <iostream>
#include <istream>
#include <memory>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include<unistd.h>
#include <netdb.h>
#include "client.hpp"
#include "response.hpp"
#include <cstdio>




class Request {
private:
    std::string host, api;

    addrinfo *adres;
    int sock;

public:
    Request(const std::string &url_path_p, int port);
    
    ~Request();
    
    template<typename T>
    static auto execute(client<T> &client, int port = 80) -> response {
        Request req(client.get_url(), port);
        req.send(client.prepare_request_str());
        return req.receive();
    }

private:
    void process_route(std::string url_path);

    void estabilish_connection(int port);


    void send(const std::string &request);
    auto receive() -> response;

    auto parse_raw_response(const std::string &buf) -> response;
};