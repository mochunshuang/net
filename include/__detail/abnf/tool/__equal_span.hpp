#pragma once

#include "../__detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr auto equal_span(__detail::span_param_in a,
                              __detail::span_param_in b) noexcept
    {
        const auto k_size = a.size();
        if (k_size != b.size())
            return false;
        for (std::size_t i = 0; i < k_size; ++i)
        {
            if (a[i] != b[i])
                return false;
        }
        return true;
    }

}; // namespace mcs::abnf::tool