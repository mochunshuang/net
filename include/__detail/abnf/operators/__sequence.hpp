#pragma once

#include "./__operable_rule.hpp"
#include <utility>

namespace mcs::abnf::operators
{
    template <operable_rule... Rule>
    struct sequence
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::const_parser_ctx ctx) const noexcept
            -> detail::consumed_result
        {
            std::size_t total = 0;
            detail::parser_ctx copy_ctx = ctx;
            auto apply_rule = [&]<typename R>(R &&r) constexpr noexcept -> bool {
                if (auto res = std::forward<R>(r)(copy_ctx))
                {
                    total += *res;
                    copy_ctx.cur_index += *res;
                    return true;
                }
                return false;
            };
            return static_cast<bool>((apply_rule(Rule{}) && ...))
                       ? detail::make_consumed_result(total)
                       : std::nullopt;
        }
    };
}; // namespace mcs::abnf::operators