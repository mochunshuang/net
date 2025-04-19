#pragma once

#include "./__relative_part.hpp"
#include "__query.hpp"

namespace mcs::abnf::http
{
    // partial-URI = relative-part [ "?" query ]
    constexpr abnf_result auto partial_URI(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        const auto k_size = sp.size();

        const auto k_index = tool::find_index(sp, '?');

        if (k_index == k_size)
            return relative_part(sp)
                       ? builder::success(span{.start = 0, .count = k_size}, invalid_span)
                       : builder::fail(0);

        return relative_part(sp.first(k_index)) && query(sp.subspan(k_index + 1))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = k_size - 1 - k_index})
                   : builder::fail(0);
    }
}; // namespace mcs::abnf::http