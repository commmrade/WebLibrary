#include <stdexcept>
#include <string>

class HttpRequest;

class Query {
private:
    std::string content{};
    friend HttpRequest;

    Query() = default;
public:
    

    [[nodiscard]]
    std::string as_str() {
        return content;
    }

    template <typename T>
    [[nodiscard]]
    T as() const {
        return content;
    } 
    template <>
    [[nodiscard]]
    inline float as<float>() const {
        if (content.empty()) {
            throw std::invalid_argument("Could not be converted");
        }
    
        auto converted_val = std::stof(content);
        return converted_val;
    }
    template <>
    [[nodiscard]]
    inline int as<int>() const {
        if (content.empty()) {
            throw std::invalid_argument("Could not be converted");
        }
    
        auto converted_val = std::stoi(content);
        return converted_val;
    }
    template <>
    [[nodiscard]]
    inline long long as<long long>() const {
        if (content.empty()) {
            throw std::invalid_argument("Could not be converted");
        }
    
        auto converted_val = std::stoi(content);
        return converted_val;
    }
    template <>
    [[nodiscard]]
    inline double as<double>() const {
        if (content.empty()) {
            throw std::invalid_argument("Could not be converted");
        }

        auto converted_val = std::stod(content);
        return converted_val; 
    }  
    template<>
    [[nodiscard]]
    inline const char* as<const char*>() const {
        return content.c_str();
    }
};

