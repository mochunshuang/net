#pragma once

#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // RWS = 1*( SP / HTAB )
    constexpr abnf_result auto RWS(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);
        return OWS(sp);
    }
}; // namespace mcs::abnf::http