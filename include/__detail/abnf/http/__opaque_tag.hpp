#pragma once

#include "./__etagc.hpp"

namespace mcs::abnf::http
{
    // opaque-tag = DQUOTE *etagc DQUOTE
    using opaque_tag = sequence<DQUOTE, zero_or_more<etagc>, DQUOTE>;
}; // namespace mcs::abnf::http