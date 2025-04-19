#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::http
{
    /*
    other-range = 1*( %x21-2B ; '!'-'+'
    / %x2D-7E ; '-'-'~'
    )
    */
    constexpr abnf_result auto other_range(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);
        for (std::size_t i{0}; i < k_size; ++i)
        {
            if (const auto &c = sp[i]; (c >= '!' && c <= '+') || (c >= '-' && c <= '~'))
                continue;
            return simple_result::fail(i);
        }
        return simple_result::success();
    }
}; // namespace mcs::abnf::http