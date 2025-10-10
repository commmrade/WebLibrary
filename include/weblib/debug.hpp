#pragma once
#ifdef DEBUG
#include <iostream>
#endif
namespace weblib {
namespace debug
{

template <typename... Args>
void log_info([[maybe_unused]] Args... args)
{
#ifdef DEBUG
    std::cout << "[INFO]: ";
    ((std::cout << args), ..., (std::cout << std::endl));
#endif
}

template <typename... Args>
void log_warn([[maybe_unused]] Args... args)
{
#ifdef DEBUG
    std::cout << "[WARNING]: ";
    ((std::cout << args), ..., (std::cout << std::endl));
#endif
}

template <typename... Args>
void log_error([[maybe_unused]] Args... args)
{
#ifdef DEBUG
    std::cout << "[ERROR]: ";
    ((std::cout << args), ..., ((std::cout << std::flush), perror(" ")));
#endif
}

} // namespace debug
} // namespace weblib