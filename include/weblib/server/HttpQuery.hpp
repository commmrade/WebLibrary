#include "weblib/server/Query.hpp"
#include <unordered_map>
#include <string>
#include <vector>

namespace weblib {

class HttpQuery
{
  private:
    std::unordered_map<std::string, std::string> m_parameters;

  public:
    HttpQuery() = default;
    explicit HttpQuery(const std::string &raw_http, const std::vector<std::string> &parameters,
                       const std::string &serv_path)
    {
        parse_from_string(raw_http, parameters, serv_path);
    }

    auto get_query(const std::string &query_name) const -> Query;
    [[nodiscard]]
    auto get_queries() const -> const std::unordered_map<std::string, std::string>&
    {
        return m_parameters;
    }

    void parse_from_string(const std::string              &raw_http,
                           const std::vector<std::string> &parameters,
                           std::string_view template_path);
};
} // namespace weblib