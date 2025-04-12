#pragma once

#include "./__unreserved.hpp"
#include "./__sub_delims.hpp"

#include "../tool/__split_span.hpp"

namespace mcs::ABNF::URI
{
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    constexpr bool IPvFuture(default_span_t sp) noexcept
    {
        //
        const auto k_size = sp.size();
        if (k_size < 4)
            return false;

        static_assert(unreserved('.'));
        static_assert(not HEXDIG('.'));
        static_assert(not HEXDIG('v'));

        if (sp[0] != 'v')
            return true;

        const auto [front, tail] = tool::split_span_first(sp, '.');

        // check front: "v" 1*HEXDIG
        const auto k_f_size = front.size();
        if (k_f_size < 2)
            return false;
        for (size_t i = 1; i < k_f_size; ++i)
        {
            if (not HEXDIG(front[i]))
                return false;
        }

        // check tail: 1*( unreserved / sub-delims / ":" )
        const auto k_t_size = tail.size();
        if (k_t_size < 1)
            return false;
        for (size_t i = 0; i < k_t_size; ++i)
        {
            const auto &c = tail[i];
            if (!unreserved(c) && !sub_delims(c) && c != ':')
                return false;
        }
        return true;
    }
}; // namespace mcs::ABNF::URI