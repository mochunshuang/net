#pragma once

#include "../__detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr auto find_index(__detail::span_param_in sp,
                              __detail::octet_param_in c) noexcept
    {
        const auto k_size = sp.size();
        auto index{k_size};
        for (std::size_t i{0}; i < k_size; ++i)
        {
            if (sp[i] == c)
            {
                index = i;
                break;
            }
        }
        return index;
    }
    constexpr auto find_index_from_end(__detail::span_param_in sp,
                                       __detail::octet_param_in c) noexcept
    {
        const auto k_size = sp.size();
        auto index{k_size};
        for (std::size_t i = k_size; i-- > 0;)
        {
            if (sp[i] == c)
            {
                index = i;
                break;
            }
        }
        return index;
    }

}; // namespace mcs::abnf::tool