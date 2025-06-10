#pragma once
#include <optional>
#include <ranges>
#include <unordered_map>
#include <string>



template <typename Map>
class InsideView {
private:
    const Map& headers;
public:
    explicit InsideView(const Map& headers) : headers(headers) {}
    using iter_type = typename Map::const_iterator;

    [[nodiscard]]
    std::optional<typename iter_type::value_type> get(const std::string& key) const {
        auto pos = headers.find(key);
        if (pos == headers.end()) {
            return std::nullopt;
        }
        return *pos;
    }

    [[nodiscard]]
    iter_type begin() {
        return iter_type{headers.cbegin()};
    }
    [[nodiscard]]
    iter_type end() {
        return iter_type{headers.cend()};
    }
    iter_type begin() const {
        return iter_type{headers.cbegin()};
    }
    iter_type end() const {
        return iter_type{headers.cend()};
    }

    [[nodiscard]]
    iter_type cbegin() const {
        return iter_type{headers.cbegin()};
    }
    [[nodiscard]]
    iter_type cend() const {
        return iter_type{headers.cend()};
    }
};
class Cookie;
using HeaderView = InsideView<std::unordered_map<std::string, std::string>>;
using CookieView = InsideView<std::unordered_map<std::string, Cookie>>;
using QueryView = HeaderView;