#include <stdexcept>
#include <string>

class HttpRequest;

class Query {
private:
    std::string m_content{};
    friend HttpRequest;

    Query() = default;
public:
    

    [[nodiscard]]
    std::string as_str() {
        return m_content;
    }

    template <typename T>
    [[nodiscard]]
    T as() const {
        return m_content;
    } 

};

template <>
[[nodiscard]]
inline float Query::as<float>() const {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stof(m_content);
    return converted_val;
}
template <>
[[nodiscard]]
inline int Query::as<int>() const {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stoi(m_content);
    return converted_val;
}
template <>
[[nodiscard]]
inline long long Query::as<long long>() const {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stoll(m_content);
    return converted_val;
}
template <>
[[nodiscard]]
inline double Query::as<double>() const {
    if (m_content.empty()) {
        throw std::invalid_argument("Could not be converted");
    }

    auto converted_val = std::stod(m_content);
    return converted_val;
}
template<>
[[nodiscard]]
inline const char* Query::as<const char*>() const {
    return m_content.c_str();
}
