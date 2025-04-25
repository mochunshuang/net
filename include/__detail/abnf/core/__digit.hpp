#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::core
{
    // DIGIT          =  %x30-39
    struct DIGIT
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            static_assert('0' == 0x30);    // NOLINT
            return (c >= '0' && c <= '9'); // NOLINT
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core