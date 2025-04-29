#pragma once

#include "./__types.hpp"
#include <concepts>

namespace mcs::abnf::detail
{

    template <class Rule>
    concept enable_rule = std::derived_from<typename Rule::rule_concept, rule_t> &&
                          requires(Rule &&rule, detail::parser_ctx &ctx) {
                              {
                                  std::forward<Rule>(rule)(ctx)
                              } noexcept -> std::same_as<detail::consumed_result>;
                              requires std::is_default_constructible_v<Rule>;
                          };

    template <typename Rule>
    concept rule = enable_rule<std::remove_cvref_t<Rule>> &&
                   std::move_constructible<std::remove_cvref_t<Rule>> && // movable
                   std::constructible_from<std::remove_cvref_t<Rule>, Rule>;

}; // namespace mcs::abnf::detail
