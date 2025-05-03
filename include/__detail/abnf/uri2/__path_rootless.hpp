#pragma once

#include "./__segment.hpp"
#include "./__segment_nz.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using path_rootless_rule =
            sequence<segment_nz, zero_or_more<sequence<CharSensitive<'/'>, segment>>>;
    };
    // path-rootless = segment-nz *( "/" segment )
    struct path_rootless : SimpleRule<path_rootless, rules::path_rootless_rule>
    {
        using SimpleRule<path_rootless, rules::path_rootless_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri