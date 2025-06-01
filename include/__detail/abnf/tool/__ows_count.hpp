#pragma once

#include "../detail/__types.hpp"
#include "../core/__sp.hpp"
#include "../core/__htab.hpp"

namespace mcs::abnf::tool
{

    static constexpr auto ows_count(detail::parser_ctx_ref ctx) noexcept
    {
        size_t count_ = 0;
        while (true)
        {
            auto index = ctx.cur_index + count_;
            if (index < ctx.end_index && (core::sp_value == ctx.root_span[index] ||
                                          core::htab_value == ctx.root_span[index]))
            {
                ++count_;
                continue;
            }
            break;
        }
        return count_;
    };
}; // namespace mcs::abnf::tool