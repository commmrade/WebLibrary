#pragma once
#include <cstdint>
#include <string>
#include <utility>


enum class RequestType : std::uint8_t;

template <class T>
void hash_combine(std::size_t & s, const T & v)
{
  std::hash<T> h;
  s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2); // I am not even gonna try to explain it
}

namespace std {
    template<>
    struct hash<std::pair<std::string, RequestType>> {
        size_t operator()(const std::pair<std::string, RequestType> &x) const noexcept {
            size_t res = 0;
            hash_combine(res, x.first);
            hash_combine(res, x.second);

            return res;
        }
    };
    template<>
    struct hash<RequestType> {
        size_t operator()(const RequestType &x) const noexcept {
            auto hash = std::hash<int>()(static_cast<int>(x));

            return hash;
        }
    };
}
