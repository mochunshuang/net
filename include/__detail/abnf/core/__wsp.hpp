#pragma once

#include "./__sp.hpp"
#include "./__htab.hpp"

namespace mcs::abnf::core
{
    // WSP            =  SP / HTAB; white space
    struct WSP
    {
        using rule_concept = abnf::rule_t;

        static constexpr bool parse(octet_in c) noexcept
        {
            return SP::parse(c) || HTAB::parse(c);
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (!ctx.empty() && parse(ctx.root_span[ctx.cur_index]))
                return 1;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core