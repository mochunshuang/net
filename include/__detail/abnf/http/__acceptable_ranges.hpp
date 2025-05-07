#pragma once

#include "./__range_unit.hpp"
#include "./__ows.hpp"

namespace mcs::abnf::http
{
    // acceptable-ranges = range-unit *( OWS "," OWS range-unit )
    using acceptable_ranges =
        sequence<range_unit, zero_or_more<sequence<OWS, Char<','>, OWS, range_unit>>>;
}; // namespace mcs::abnf::http