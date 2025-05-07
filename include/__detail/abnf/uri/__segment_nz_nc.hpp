#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    //  segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
    //      ; non - zero - length segment without any colon ":"
    using segment_nz_nc =
        one_or_more<alternative<unreserved, pct_encoded, sub_delims, Char<'@'>>>;
}; // namespace mcs::abnf::uri