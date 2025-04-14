#pragma once

#include "../__core_rules.hpp"

namespace mcs::abnf::uri
{
    // h16           = 1*4HEXDIG
    constexpr auto h16(span_param_in sp) noexcept -> abnf_result auto
    {
        using builder = result_builder<result<1>>;
        const auto k_size = sp.size();
        if (k_size == 0 || k_size > 4)
            return builder::fail(0);

        for (size_t i{}; i < k_size; ++i)
        {
            if (not HEXDIG(sp[i]))
                return builder::fail(i);
        }
        return builder::success(span{.start = 0, .count = k_size});
    }
}; // namespace mcs::abnf::uri