#pragma once

#include "./__ows.hpp"
#include "./__range_spec.hpp"

namespace mcs::abnf::http
{
    // range-set = range-spec *( OWS "," OWS range-spec )
    using range_set =
        sequence<range_spec, zero_or_more<sequence<OWS, Char<','>, OWS, range_spec>>>;

}; // namespace mcs::abnf::http