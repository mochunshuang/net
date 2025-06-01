#pragma once

#include "../detail/__types.hpp"

namespace mcs::abnf::tool
{
    struct find_char_parm
    {
        size_t start;
        detail::octet c;
    };
    static constexpr auto find_char(detail::parser_ctx_ref ctx,
                                    find_char_parm parm) noexcept
    {
        for (size_t i{parm.start}; i < ctx.end_index; ++i)
        {
            if (ctx.root_span[i] == parm.c)
                return i;
        }
        return ctx.end_index;
    };
}; // namespace mcs::abnf::tool