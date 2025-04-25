#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // CTL            =  %x00-1F / %x7F
    struct CTL
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            return c <= 0X1F || c == 0x7F; // NOLINT
        }
        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core