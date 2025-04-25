#pragma once

#include "../__abnf.hpp"
#include "./__cr.hpp"
#include "./__lf.hpp"

namespace mcs::abnf::core
{
    struct CRLF_TYPE
    {
    };

    // CRLF           =  CR LF
    struct CRLF
    {
        using rule_concept = rule_t;

        static constexpr bool parse(octet_in cr, octet_in lf) noexcept
        {
            return CR::parse(cr) && LF::parse(lf);
        }

        static constexpr auto parse(const_parser_ctx ctx) noexcept -> consumed_result
        {
            if (ctx.size() > 1 &&
                parse(ctx.root_span[ctx.cur_index], ctx.root_span[ctx.cur_index + 1]))
                return 2;
            return std::nullopt;
        }
    };

}; // namespace mcs::abnf::core