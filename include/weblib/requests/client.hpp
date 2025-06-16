// SPDX-License-Identifier: MIT
// Copyright (c) 2025 Klewy
#pragma once
#include<iostream>
#include<string>
#include<unordered_map>


template<typename Derived>
class client {
protected:
    std::string url;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
public:
    
    ~client() {}

    std::string prepare_request_str() {
        return static_cast<Derived*>(this)->prepare_request_str();
    }

    void set_header(const std::string &key, const std::string &value);

    std::string get_url() { return url; }

private:
    client (const std::string &url);
    friend Derived;
};

class get_client final : public client<get_client> {
public:
    get_client(const std::string &url);

    std::string prepare_request_str();
};



class post_client final : public client<post_client> {
private:
    std::string body;
public:
    post_client(const std::string &url);

    std::string prepare_request_str();
    void set_body(const std::string &str);
};


class put_client final : public client<put_client> {
private:
    std::string body;
public:
    put_client(const std::string &url);

    std::string prepare_request_str();

    void set_body(const std::string &str);
};


class delete_client final : public client<delete_client> {
private:
    std::string body;
public:
    delete_client(const std::string &url);

    std::string prepare_request_str();

    void set_body(const std::string &str);
};