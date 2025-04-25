#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // BIT            =  "0" / "1"
    struct BIT
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            return c == '1' || c == '0';
        }
        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core