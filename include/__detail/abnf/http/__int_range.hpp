#pragma once

#include "./__first_pos.hpp"
#include "./__last_pos.hpp"

namespace mcs::abnf::http
{
    // int-range = first-pos "-" [ last-pos ]
    constexpr abnf_result auto int_range(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        static_assert(not DIGIT('-'));
        const std::size_t k_index = tool::find_index(sp, '-');
        const std::size_t k_szie = sp.size();
        if (k_index == k_szie)
            return builder::fail(0);
        if (k_index == k_szie - 1 && first_pos(sp.first(k_index)))
            return builder::success(span{.start = 0, .count = k_index}, invalid_span);

        return first_pos(sp.first(k_index)) && last_pos(sp.subspan(k_index + 1))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = k_szie - k_index - 1})
                   : builder::fail(0);
    }
}; // namespace mcs::abnf::http