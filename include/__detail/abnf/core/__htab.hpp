#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // HTAB           =  %x09; horizontal tab
    struct HTAB
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            constexpr octet k_htb = 0x09;
            return c == k_htb;
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core