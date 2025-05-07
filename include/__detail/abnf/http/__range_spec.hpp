#pragma once

#include "./__int_range.hpp"
#include "./__suffix_range.hpp"
#include "./__other_range.hpp"

namespace mcs::abnf::http
{
    // range-spec = int-range / suffix-range / other-range
    using range_spec = alternative<int_range, suffix_range, other_range>;

}; // namespace mcs::abnf::http