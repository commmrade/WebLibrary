#include "weblib/server/HttpQuery.hpp"
#include "weblib/server/Utils.hpp"
#include <ranges>

void HttpQuery::parse_from_string(const std::string              &raw_http,
                                  const std::vector<std::string> &parameters,
                                  std::string_view               template_path)
{
    if (parameters.empty())
    {
        return;
    }

    auto param_iter =
        parameters.begin(); // m_param_names stores id, user from api/{id}/{user} (example)
    if (param_iter == parameters.end())
    {
        throw std::runtime_error("Malformed http m_request");
    }

    size_t const endpoint_start = raw_http.find('/');
    if (endpoint_start == std::string::npos)
    {
        throw std::runtime_error("Wtf");
    }
    std::string_view const path{raw_http.data() + endpoint_start + 1,
                                raw_http.find("HTTP") - endpoint_start -
                                    2}; // additional 1 taking a space into account

    // Slash handling
    template_path.remove_prefix(1);

    // Processing path arguments
    auto t_path_slash = template_path.substr(0, template_path.find('?'));
    auto t_path_params =
        t_path_slash | std::views::split('/') |
        std::views::transform(
            [](auto &&range)
            {
                return std::string_view{range.begin(),
                                        static_cast<size_t>(std::ranges::distance(range))};
            }) |
        std::ranges::to<std::vector>();

    auto path_slash = path.substr(0, path.find('?'));
    auto path_slash_params =
        path_slash | std::views::split('/') |
        std::views::transform(
            [](auto &&range)
            {
                return std::string_view{range.begin(),
                                        static_cast<size_t>(std::ranges::distance(range))};
            }) |
        std::ranges::to<std::vector>();
    if (t_path_params.size() != path_slash_params.size())
    {
        throw std::runtime_error("template_path slash args size != Path slash args size");
    }

    for (const auto &&[pattern_arg, path_arg] :
         std::views::zip(t_path_params, path_slash_params))
    {
        if (pattern_arg.contains('{'))
        { // Если есть скобка, значит параметр шаблонный
            if (param_iter == parameters.end())
            {
                throw std::runtime_error("Malformed http m_request");
            }
            m_parameters.emplace(*param_iter, path_arg);
            ++param_iter;
        }
    }

    
    auto t_path_question_pos = template_path.find('?');
    auto t_path_query        = template_path.substr(
        t_path_question_pos == std::string::npos ? template_path.size() : t_path_question_pos + 1);

    auto path_question_pos    = path.find('?');
    auto path_query =
        path.substr(path_question_pos == std::string::npos ? path.size() : path_question_pos + 1);

    auto t_path_query_params =
        t_path_query | std::views::split('&') |
        std::views::transform(
            [](auto &&range)
            {
                return std::string_view{range.begin(),
                                        static_cast<size_t>(std::ranges::distance(range))};
            }) |
        std::ranges::to<std::vector>();
    auto path_query_params =
        path_query | std::views::split('&') |
        std::views::transform(
            [](auto &&range)
            {
                return std::string_view{range.begin(),
                                        static_cast<size_t>(std::ranges::distance(range))};
            }) |
        std::ranges::to<std::vector>();
    if (t_path_query_params.size() != path_query_params.size())
    {
        throw std::runtime_error("template_path query args size != Path query args size");
    }

    auto split_kv_query =
        [](std::string_view key_value) -> std::pair<std::string_view, std::string_view>
    {
        auto k_v = key_value | std::views::split('=') |
                   std::views::transform(
                       [](auto &&range)
                       {
                           return std::string_view{
                               range.begin(), static_cast<size_t>(std::ranges::distance(range))};
                       }) |
                   std::ranges::to<std::vector>();
        if (k_v.size() != 2)
        {
            throw std::runtime_error("Ill-formed m_request");
        }
        return {k_v.front(), k_v.back()};
    };

    for (const auto &&[t_path_kv, path_kv] : std::views::zip(t_path_query_params, path_query_params))
    {
        auto [t_path_name, t_path_value] = split_kv_query(t_path_kv);
        auto [path_name, path_value]     = split_kv_query(path_kv);
        if (t_path_value.contains('{'))
        { // '{' means Template param
            if (param_iter == parameters.end())
            {
                throw std::runtime_error("Malformed http m_request");
            }
            m_parameters.emplace(*param_iter, path_value);
            ++param_iter;
        }
        else
        { // Means a static query parameter, not dynamic
            m_parameters.emplace(path_name, path_value);
        }
    }
}

auto HttpQuery::get_query(const std::string &query_name) const -> Query
{
    Query query;
    auto  pos = m_parameters.find(utils::to_lowercase_str(query_name));
    if (pos != m_parameters.end())
    { // If header exists
        query.m_content = pos->second;
    }
    return query;
}
