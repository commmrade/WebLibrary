#include <unordered_map>
#include <string>
#include "Cookie.hpp"

namespace weblib
{

class HttpHeaders
{
  private:
    std::unordered_map<std::string, std::string> m_headers;
    std::unordered_map<std::string, Cookie>      m_cookies;
    void extract_headers_from_str(const std::string &raw_headers);
    void parse_cookie(std::string_view cookie);

  public:
    HttpHeaders() = default;
    explicit HttpHeaders(const std::string &raw_http) { extract_headers_from_str(raw_http); }

    void parse_from_string(const std::string &raw_http) { extract_headers_from_str(raw_http); }

    [[nodiscard]]
    auto get_header(const std::string &header_name) const -> std::optional<std::string>;
    [[nodiscard]]
    auto get_headers() const -> const std::unordered_map<std::string, std::string> &
    {
        return m_headers;
    }

    void set_header(const std::string &name, std::string_view value) { m_headers[name] = value; }

    [[nodiscard]]
    auto get_cookie(const std::string &name) const -> std::optional<Cookie>;

    [[nodiscard]]
    auto get_cookies() const -> const std::unordered_map<std::string, Cookie> &
    {
        return m_cookies;
    }
};
} // namespace weblib