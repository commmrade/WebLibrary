#include "weblib/server/HeaderView.hpp"
#include "weblib/server/Query.hpp"
#include <unordered_map>
#include <string>
#include <vector>

class HttpQuery
{
  private:
    std::unordered_map<std::string, std::string> m_parameters;

  public:
    HttpQuery() = default;
    explicit HttpQuery(const std::string &request_str, const std::vector<std::string> &param_names,
                       const std::string &serv_path)
    {
        parse_from_string(request_str, param_names, serv_path);
    }

    auto get_query(const std::string &query_name) const -> Query;
    [[nodiscard]]
    auto get_queries() const -> QueryView
    {
        return QueryView{m_parameters};
    }

    void parse_from_string(const std::string              &request_str,
                           const std::vector<std::string> &param_names,
                           std::string_view template_path);
};
