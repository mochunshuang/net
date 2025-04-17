#pragma once

#include "./__tchar.hpp"

namespace mcs::abnf::http
{
    // token = 1*tchar
    constexpr abnf_result auto token(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);
        for (std::size_t i{0}; i < k_size; ++i)
        {
            if (not tchar(sp[i]))
                return simple_result::fail(i);
        }
        return simple_result::success();
    }

}; // namespace mcs::abnf::http