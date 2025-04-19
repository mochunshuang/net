#pragma once

#include "./__range_unit.hpp"
#include "./__range_set.hpp"

namespace mcs::abnf::http
{
    // ranges-specifier = range-unit "=" range-set
    constexpr abnf_result auto ranges_specifier(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        static_assert(not tchar('='));

        const auto k_index = tool::find_index(sp, '=');
        const auto k_size = sp.size();
        if (k_index == k_size ||
            not(range_unit(sp.first(k_index)) && range_set(sp.subspan(k_index + 1))))
            return builder::fail(0);

        return builder::success(
            span{.start = 0, .count = k_index},
            span{.start = k_index + 1, .count = k_size - k_index - 1});
    }
}; // namespace mcs::abnf::http