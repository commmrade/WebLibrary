#pragma once
#include <ranges>
#include <unordered_map>
#include <string>


class HeaderView : std::ranges::view_interface<HeaderView> {
private:
    const std::unordered_map<std::string, std::string>& headers;
public:
    HeaderView(const std::unordered_map<std::string, std::string>& headers) : headers(headers) {}
    
    struct HeaderIterator {
    private:
        std::unordered_map<std::string, std::string>::const_iterator iter;
    public:
        using iter_type = std::unordered_map<std::string, std::string>::const_iterator;
        using iterator_category = typename iter_type::iterator_category;
        using value_type = std::pair<std::string_view, std::string_view>;
        using reference = value_type;
        using pointer = void;
        using difference_type = typename iter_type::difference_type;

        HeaderIterator(std::unordered_map<std::string, std::string>::const_iterator iter) : iter(iter) {}

        HeaderIterator& operator++() {
            ++iter;
            return *this;
        }
        HeaderIterator operator++(int) {
            auto temp = *this;
            ++iter;
            return temp;
        }
        value_type operator*() const {
            return value_type{iter->first, iter->second};
        }
        bool operator==(const HeaderIterator& rhs) const {
            return iter == rhs.iter;
        }
        bool operator!=(const HeaderIterator& rhs) const {
            return !(*this == rhs);
        }
    };

    [[nodiscard]]
    HeaderIterator begin() {
        return HeaderIterator{headers.cbegin()};
    }
    [[nodiscard]]
    HeaderIterator end() {
        return HeaderIterator{headers.cend()};
    }
    HeaderIterator begin() const {
        return HeaderIterator{headers.cbegin()};
    }
    HeaderIterator end() const {
        return HeaderIterator{headers.cend()};
    }

    [[nodiscard]]
    const HeaderIterator cbegin() const {
        return HeaderIterator{headers.cbegin()};
    }
    [[nodiscard]]
    const HeaderIterator cend() const {
        return HeaderIterator{headers.cend()};
    }
};
