#pragma once

#include "./__suffix_length.hpp"

namespace mcs::abnf::http
{
    // suffix-range = "-" suffix-length
    constexpr abnf_result auto suffix_range(span_param_in sp) noexcept
    {
        if (sp.size() == 0 || sp[0] != '-')
            return simple_result::fail(0);
        return suffix_length(sp.subspan(1));
    }
}; // namespace mcs::abnf::http