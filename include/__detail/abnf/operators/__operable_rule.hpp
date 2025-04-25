#pragma once

#include "../detail/__concept.hpp"
#include "../detail/__types.hpp"

namespace mcs::abnf::operators
{
    template <typename Rule>
    concept operable = requires(Rule &&rule, detail::const_parser_ctx ctx) {
        {
            std::forward<Rule>(rule)(ctx)
        } noexcept -> std::same_as<detail::consumed_result>;
        requires std::is_default_constructible_v<Rule>;
    };

    template <typename Rule>
    concept operable_rule = detail::ruleer<Rule> && operable<Rule>;

}; // namespace mcs::abnf::operators