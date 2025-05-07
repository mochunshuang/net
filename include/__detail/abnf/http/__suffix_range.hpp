#pragma once

#include "./__suffix_length.hpp"

namespace mcs::abnf::http
{
    // suffix-range = "-" suffix-length
    using suffix_range = sequence<Char<'-'>, suffix_length>;
}; // namespace mcs::abnf::http