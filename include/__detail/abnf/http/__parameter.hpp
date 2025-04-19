#pragma once

#include "./__parameter_name.hpp"
#include "./__parameter_value.hpp"

namespace mcs::abnf::http
{
    // parameter = parameter-name "=" parameter-value
    constexpr abnf_result auto parameter(span_param_in sp) noexcept
    {
        using builder = result_builder<result<2>>;
        const auto k_size = sp.size();
        if (k_size == 0)
            return builder::fail(0);
        const auto k_index = tool::find_index(sp, '=');
        if (k_index == k_size)
            return builder::fail(0);

        return parameter_name(sp.first(k_index)) &&
                       parameter_value(sp.subspan(k_index + 1))
                   ? builder::success(
                         span{.start = 0, .count = k_index},
                         span{.start = k_index + 1, .count = k_size - k_index - 1})
                   : builder::fail(0);
    }

}; // namespace mcs::abnf::http