#pragma once

#include "./__operable_rule.hpp"
#include "./__transaction.hpp"

namespace mcs::abnf::operators
{
    template <size_t Min, size_t Max, operable_rule Rule>
    struct repetition
    {
        using rule_concept = detail::rule_t;

        constexpr auto operator()(detail::parser_ctx &ctx) const noexcept
            -> detail::consumed_result
        {
            transaction trans{ctx};
            std::size_t old_index = ctx.cur_index;
            std::size_t count = 0;
            while (count <= Max && ctx.valid())
            {
                if (auto ret = Rule{}(ctx); not ret)
                    break;
                ++count;
            }
            return (count >= Min)
                       ? (trans.commit(),
                          detail::make_consumed_result(ctx.cur_index - old_index))
                       : std::nullopt;
        };
    };
}; // namespace mcs::abnf::operators