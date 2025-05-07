#pragma once

#include "./__incl_range.hpp"
#include "./__complete_length.hpp"

namespace mcs::abnf::http
{
    // range-resp = incl-range "/" ( complete-length / "*" )
    using range_resp =
        sequence<incl_range, Char<'/'>, alternative<complete_length, Char<'*'>>>;

}; // namespace mcs::abnf::http