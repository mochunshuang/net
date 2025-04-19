#pragma once

#include "./__int_range.hpp"
#include "./__suffix_range.hpp"
#include "./__other_range.hpp"

namespace mcs::abnf::http
{
    // range-spec = int-range / suffix-range / other-range
    constexpr abnf_result auto range_spec(span_param_in sp) noexcept
    {
        return int_range(sp) || suffix_range(sp) || other_range(sp);
    }

}; // namespace mcs::abnf::http