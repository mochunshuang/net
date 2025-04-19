#pragma once

#include "./__find_index.hpp"

namespace mcs::abnf::tool
{
    constexpr auto split_span_first(__detail::span_param_in sp,
                                    __detail::octet_param_in c) noexcept
    {
        // NOTE: first(i): i < k_size is ok
        // NOTE: subspan(i+1): i+1 <=k_size is ok
        if (const auto k_index = find_index(sp, c); k_index != sp.size())
            return std::pair{sp.first(k_index), sp.subspan(k_index + 1)};
        return std::pair{sp, __detail::empty_span_param}; // not find c in sp
    }
    constexpr auto split_span_last(__detail::span_param_in sp,
                                   __detail::octet_param_in c) noexcept
    {
        if (const auto k_index = find_index_from_end(sp, c); k_index != sp.size())
            return std::pair{sp.first(k_index), sp.subspan(k_index + 1)};
        return std::pair{__detail::empty_span_param, sp}; // not find c in sp
    }
}; // namespace mcs::abnf::tool