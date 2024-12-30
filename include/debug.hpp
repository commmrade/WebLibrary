#pragma once
#include <iostream>


namespace debug {


template<typename... Args>
void log_info(Args... args) {
#ifdef DEBUG
    std::cout << "[INFO]: ";
    ((std::cout << args), ..., (std::cout << std::endl));
#endif
}


template<typename... Args>
void log_warn(Args... args) {
#ifdef DEBUG
    std::cout << "[WARNING]: ";
    ((std::cout << args), ..., (std::cout << std::endl));
#endif
}


template<typename... Args>
void log_error(Args... args) {
#ifdef DEBUG
    std::cout << "[ERROR]: ";
    ((std::cout << args), ..., ((std::cout << std::flush), perror(" ")));
#endif
}

}
