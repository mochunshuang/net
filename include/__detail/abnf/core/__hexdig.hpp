#pragma once

#include "../__abnf.hpp"
#include "./__digit.hpp"

namespace mcs::abnf::core
{
    // NOTE: 不区分大小写除非特别说明
    // HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    struct HEXDIG
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            return DIGIT::parse(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core