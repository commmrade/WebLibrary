#include <requests/client.hpp>

template<typename T>
client<T>::client (const std::string &url) : url(url) {}


template<typename T>
void client<T>::set_header(const std::string &key, const std::string &value) {
    headers[key] = value;
}




get_client::get_client(const std::string &url) : client(url) { }

std::string get_client::prepare_request_str() {

    if (url.find("https://") != url.npos) {
        url.replace(url.find("https://"), 8, "");
    } else if (url.find("http://") != url.npos) {
        url.replace(url.find("http://"), 7, "");
    }
    std::string host = url.substr(0, url.find("/"));
    std::string api = url.substr(url.find("/"));
    

    std::string request_query{};
    request_query += "GET " + api + " HTTP/1.1\r\n";
    request_query += "Host: " + host + "\r\n";
    for (auto start = headers.begin(); start != headers.end(); start++) {
        request_query += (*start).first + ": " + (*start).second;
    }
    request_query += "Connection: close\r\n\r\n";

    return request_query;
}



post_client::post_client(const std::string &url) : client(url) { }

std::string post_client::prepare_request_str() {
    if (url.find("https://") != url.npos) {
        url.replace(url.find("https://"), 8, "");
    } else if (url.find("http://") != url.npos) {
        url.replace(url.find("http://"), 7, "");
    }
    std::string host = url.substr(0, url.find("/"));
    std::string api = url.substr(url.find("/"));
    

    std::string request_query{};
    request_query += "POST " + api + " HTTP/1.1\r\n";
    request_query += "Host: " + host + "\r\n";
    for (auto start = headers.begin(); start != headers.end(); start++) {
        request_query += (*start).first + ": " + (*start).second;
    }
    request_query += "Connection: close\r\n\r\n";
    request_query += body;

    return request_query;
}

void post_client::set_body(const std::string &str) {
    body = str;
}






put_client::put_client(const std::string &url) : client(url) { }

std::string put_client::prepare_request_str() {
    if (url.find("https://") != url.npos) {
        url.replace(url.find("https://"), 8, "");
    } else if (url.find("http://") != url.npos) {
        url.replace(url.find("http://"), 7, "");
    }
    std::string host = url.substr(0, url.find("/"));
    std::string api = url.substr(url.find("/"));
    

    std::string request_query{};
    request_query += "PUT " + api + " HTTP/1.1\r\n";
    request_query += "Host: " + host + "\r\n";
    for (auto start = headers.begin(); start != headers.end(); start++) {
        request_query += (*start).first + ": " + (*start).second;
    }
    request_query += "Connection: close\r\n\r\n";
    request_query += body;

    return request_query;
}

void put_client::set_body(const std::string &str) {
    body = str;
}





delete_client::delete_client(const std::string &url) : client(url) { }

std::string delete_client::prepare_request_str() {
    if (url.find("https://") != url.npos) {
        url.replace(url.find("https://"), 8, "");
    } else if (url.find("http://") != url.npos) {
        url.replace(url.find("http://"), 7, "");
    }
    std::string host = url.substr(0, url.find("/"));
    std::string api = url.substr(url.find("/"));
    

    std::string request_query{};
    request_query += "DELETE " + api + " HTTP/1.1\r\n";
    request_query += "Host: " + host + "\r\n";
    for (auto start = headers.begin(); start != headers.end(); start++) {
        request_query += (*start).first + ": " + (*start).second;
    }
    request_query += "Connection: close\r\n\r\n";
    request_query += body;

    return request_query;
}

void delete_client::set_body(const std::string &str) {
    body = str;
}
