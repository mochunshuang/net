#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // CHAR           =  %x01-7F
    struct CHAR
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            return c >= 0X01 && c <= 0X7F; // NOLINT
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core