#pragma once

#include "./__segment.hpp"
#include "./__segment_nz_nc.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using path_noscheme_rule =
            sequence<segment_nz_nc, zero_or_more<sequence<CharSensitive<'/'>, segment>>>;
    };
    // path-noscheme = segment-nz-nc *( "/" segment )
    struct path_noscheme : SimpleRule<path_noscheme, rules::path_noscheme_rule>
    {
    };

}; // namespace mcs::abnf::uri