#pragma once

#include "./__segment.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using path_abempty_rule = zero_or_more<sequence<CharSensitive<'/'>, segment>>;
    };
    // path-abempty  = *( "/" segment )
    struct path_abempty : SimpleRule<path_abempty, rules::path_abempty_rule>
    {
    };

}; // namespace mcs::abnf::uri