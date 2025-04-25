#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    //  VCHAR          =  %x21-7E; visible (printing) characters
    struct VCHAR
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            static_assert('!' == 0x21 && '~' == 0x7E); // NOLINT
            return c >= '!' && c <= '~';
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core