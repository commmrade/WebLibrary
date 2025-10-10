#pragma once

#include <exception>
#include <stdexcept>

namespace weblib {

struct empty_cookie : public std::exception {
    virtual const char*
    what() const noexcept {
        return "Cookie is not properly set up (name || value is empty)"; 
    }
};

struct filter_before_controller : public std::exception {
    virtual const char*
    what() const noexcept {
        return "Filter is set before a controller"; 
    }
};

struct endpoint_already_set : public std::exception {
    virtual const char*
    what() const noexcept {
        return "Endpoint is already set";
    }
};

struct handle_not_set : public std::exception {
    virtual const char*
    what() const noexcept {
        return "Handle is not set";
    }
};

struct header_parsing_error : public std::exception {
    virtual const char* what() const noexcept {
        return "Header is ill-formed";
    }
};

struct query_parsing_error : public std::runtime_error {
    query_parsing_error(const std::string &s = std::string{"Query parsing error"}) : std::runtime_error(s) {}
};

struct socket_creation_error : public std::exception {
    virtual const char* what() const noexcept {
        return "Socket creation failed";
    }
};

struct socket_flags_error : public std::exception {
    virtual const char* what() const noexcept {
        return "Could not set flags for socket";
    }
};

struct socket_bind_error : public std::exception {
    virtual const char* what() const noexcept {
        return "Failed to bind socket";
    }
};

struct writing_socket_error : public std::exception {
    virtual const char* what() const noexcept {
        return "Writing to socket failed";
    }
};

struct reading_socket_error : public std::exception {
    virtual const char* what() const noexcept {
        return "Reading from socket failed";
    }
};
} // namespace weblib;