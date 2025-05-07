#pragma once

#include "./__first_pos.hpp"
#include "./__last_pos.hpp"

namespace mcs::abnf::http
{
    // int-range = first-pos "-" [ last-pos ]
    using int_range = sequence<first_pos, Char<'-'>, last_pos>;
}; // namespace mcs::abnf::http