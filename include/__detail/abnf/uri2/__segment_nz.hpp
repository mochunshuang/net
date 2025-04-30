#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using segment_nz_rule = one_or_more<pchar>;
    };
    // segment-nz    = 1*pchar
    struct segment_nz : SimpleRule<segment_nz, rules::segment_nz_rule>
    {
    };
}; // namespace mcs::abnf::uri