#pragma once
#include <format>
#ifdef DEBUG
#include <iostream>
#endif
namespace weblib {
namespace debug
{

template <typename... Args>
void log_info([[maybe_unused]] std::format_string<Args...> fmt_str, [[maybe_unused]] Args&&... args)
{
#ifdef DEBUG
    std::cout << "[INFO]: " << std::format(fmt_str, std::forward<Args>(args)...);
#endif
}

template <typename... Args>
void log_warn([[maybe_unused]] std::format_string<Args...> fmt_str, [[maybe_unused]] Args&&... args)
{
#ifdef DEBUG
    std::cout << "[WARNING]: " << std::format(fmt_str, std::forward<Args>(args)...);
#endif
}

template <typename... Args>
void log_error([[maybe_unused]] std::format_string<Args...> fmt_str, [[maybe_unused]] Args&&... args)
{
#ifdef DEBUG
    std::cout << "[ERROR]: " << std::format(fmt_str, std::forward<Args>(args)...);
#endif
}

} // namespace debug
} // namespace weblib