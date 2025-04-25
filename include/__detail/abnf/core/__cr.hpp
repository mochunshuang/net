#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // CR             =  %x0D
    struct CR
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            constexpr octet k_lf = 0x0D;
            return c == k_lf;
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core