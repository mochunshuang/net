#pragma once

#include <concepts>

namespace mcs::abnf::detail
{
    struct rule_t
    {
    };
    template <class Rule>
    concept is_rule = std::derived_from<typename Rule::rule_concept, rule_t>;
    template <class Rule>
    concept enable_rule = is_rule<Rule>;
    template <class Rule>
    concept ruleer = enable_rule<std::remove_cvref_t<Rule>> &&
                     std::move_constructible<std::remove_cvref_t<Rule>> && // movable
                     std::constructible_from<std::remove_cvref_t<Rule>,
                                             Rule>; // copyable

}; // namespace mcs::abnf::detail
