#pragma once

#include "./__segment.hpp"
#include "./__segment_nz.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using path_absolute_rule = sequence<
            CharSensitive<'/'>,
            optional<sequence<segment_nz,
                              zero_or_more<sequence<CharSensitive<'/'>, segment>>>>>;
    };
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    struct path_absolute : SimpleRule<path_absolute, rules::path_absolute_rule>
    {
        using SimpleRule<path_absolute, rules::path_absolute_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri