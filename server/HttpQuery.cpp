#include "weblib/server/HttpQuery.hpp"
#include "weblib/server/Utils.hpp"
#include <ranges>

void HttpQuery::parse_from_string(const std::string& request_str, const std::vector<std::string>& param_names, const std::string& serv_path) {
    if (param_names.empty()) {
        return;
    }

    auto param_name_iter = param_names.begin(); // m_param_names stores id, user from api/{id}/{user} (example)
    if (param_name_iter == param_names.end()) {
        throw std::runtime_error("Malformed http m_request");
    }

    size_t const endpoint_start = request_str.find('/');
    if (endpoint_start == std::string::npos) {
        throw std::runtime_error("Wtf");
    }
    std::string_view const path{request_str.data() + endpoint_start + 1, request_str.find("HTTP") - endpoint_start - 2}; // additional 1 taking a space into account

    // Slash handling
    std::string_view pattern{serv_path};
    pattern.remove_prefix(1);

    // Processing path arguments
    auto pattern_slash = pattern.substr(0, pattern.find('?'));
    auto pattern_slash_args = pattern_slash
        | std::views::split('/')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();

    auto path_slash = path.substr(0, path.find('?'));
    auto path_slash_args = path_slash
        | std::views::split('/')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    if (pattern_slash_args.size() != path_slash_args.size()) {
        throw std::runtime_error("Pattern slash args size != Path slash args size");
    }

    for (const auto&& [pattern_arg, path_arg] : std::views::zip(pattern_slash_args, path_slash_args)) {
        if (pattern_arg.contains('{')) { // Если есть скобка, значит параметр шаблонный
            if (param_name_iter == param_names.end()) {
                throw std::runtime_error("Malformed http m_request");
            }
            m_parameters.emplace(*param_name_iter, path_arg);
            ++param_name_iter;
        }
    }

    auto pattern_question_pos = pattern.find('?');
    auto path_question_pos = path.find('?');
    auto pattern_query = pattern.substr(pattern_question_pos == std::string::npos ? pattern.size() : pattern_question_pos + 1);
    auto path_query = path.substr(path_question_pos == std::string::npos ? path.size() : path_question_pos + 1);

    auto pattern_query_args = pattern_query
        | std::views::split('&')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    auto path_query_args = path_query
        | std::views::split('&')
        | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
        | std::ranges::to<std::vector>();
    if (pattern_query_args.size() != path_query_args.size()) {
        throw std::runtime_error("Pattern query args size != Path query args size");
    }

    auto split_kv_query = [](std::string_view key_value) -> std::pair<std::string_view, std::string_view> {
        auto k_v = key_value
            | std::views::split('=')
            | std::views::transform([](auto&& range) { return std::string_view{range.begin(), static_cast<size_t>(std::ranges::distance(range))}; })
            | std::ranges::to<std::vector>();
        if (k_v.size() != 2) {
            throw std::runtime_error("Ill-formed m_request");
        }
        return {k_v.front(), k_v.back()};
    };

    for (const auto&& [pattern_kv, path_kv] : std::views::zip(pattern_query_args, path_query_args)) {
        auto [pattern_name, pattern_value] = split_kv_query(pattern_kv);
        auto [path_name, path_value] = split_kv_query(path_kv);
        if (pattern_value.contains('{')) { // Если есть скобка, значит параметр шаблонный
            if (param_name_iter == param_names.end()) {
                throw std::runtime_error("Malformed http m_request");
            }
            m_parameters.emplace(*param_name_iter, path_value);
            ++param_name_iter;
        } else { // Means a static query parameter, not dynamic
            m_parameters.emplace(path_name, path_value);
        }
    }
}

auto HttpQuery::get_query(const std::string& query_name) const -> Query {
    Query query;
    auto pos = m_parameters.find(utils::to_lowercase_str(query_name));
    if (pos != m_parameters.end()) { //If header exists
        query.m_content = pos->second;
    }
    return query;
}
