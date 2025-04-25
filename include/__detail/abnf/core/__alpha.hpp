#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z
    struct ALPHA
    {
        using rule_concept = abnf::rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core