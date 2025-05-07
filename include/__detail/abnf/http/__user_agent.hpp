#pragma once

#include "./__product.hpp"
#include "./__comment.hpp"
#include "./__rws.hpp"

namespace mcs::abnf::http
{
    // User-Agent = product *( RWS ( product / comment ) )
    using User_Agent =
        sequence<product, zero_or_more<sequence<RWS, alternative<product, comment>>>>;
}; // namespace mcs::abnf::http