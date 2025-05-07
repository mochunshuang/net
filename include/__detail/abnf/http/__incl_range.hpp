#pragma once

#include "./__first_pos.hpp"
#include "./__last_pos.hpp"

namespace mcs::abnf::http
{
    // incl-range = first-pos "-" last-pos
    using incl_range = sequence<first_pos, Char<'-'>, last_pos>;
}; // namespace mcs::abnf::http