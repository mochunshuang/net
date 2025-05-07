#pragma once

#include "./__range_unit.hpp"
#include "./__range_set.hpp"

namespace mcs::abnf::http
{
    // ranges-specifier = range-unit "=" range-set
    using ranges_specifier = sequence<range_unit, Char<'='>, range_set>;
}; // namespace mcs::abnf::http