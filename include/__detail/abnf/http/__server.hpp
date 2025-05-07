#pragma once

#include "./__product.hpp"
#include "./__comment.hpp"
#include "./__rws.hpp"

namespace mcs::abnf::http
{
    // Server = product *( RWS ( product / comment ) )
    using Server =
        sequence<product, zero_or_more<sequence<RWS, alternative<product, comment>>>>;

}; // namespace mcs::abnf::http