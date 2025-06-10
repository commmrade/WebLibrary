#pragma once
#include <optional>
#include <unordered_map>
#include <string>



template <typename Map>
class InsideView {
private:
    const Map& m_headers;
public:
    explicit InsideView(const Map& m_headers) : m_headers(m_headers) {}
    using iter_type = typename Map::const_iterator;

    [[nodiscard]]
    std::optional<typename iter_type::value_type> get(const std::string& key) const {
        auto pos = m_headers.find(key);
        if (pos == m_headers.end()) {
            return std::nullopt;
        }
        return *pos;
    }

    [[nodiscard]]
    iter_type begin() {
        return iter_type{m_headers.cbegin()};
    }
    [[nodiscard]]
    iter_type end() {
        return iter_type{m_headers.cend()};
    }
    iter_type begin() const {
        return iter_type{m_headers.cbegin()};
    }
    iter_type end() const {
        return iter_type{m_headers.cend()};
    }

    [[nodiscard]]
    iter_type cbegin() const {
        return iter_type{m_headers.cbegin()};
    }
    [[nodiscard]]
    iter_type cend() const {
        return iter_type{m_headers.cend()};
    }
};
class Cookie;
using HeaderView = InsideView<std::unordered_map<std::string, std::string>>;
using CookieView = InsideView<std::unordered_map<std::string, Cookie>>;
using QueryView = HeaderView;