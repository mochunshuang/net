#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using segment_rule = zero_or_more<pchar>;
    };
    // segment       = *pchar
    struct segment : SimpleRule<segment, rules::segment_rule>
    {
        using SimpleRule<segment, rules::segment_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri