#pragma once

#include "./__segment.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using path_abempty_rule = zero_or_more<sequence<Char<'/'>, segment>>;
    };
    // path-abempty  = *( "/" segment )
    struct path_abempty : SimpleRule<path_abempty, rules::path_abempty_rule>
    {
        using SimpleRule<path_abempty, rules::path_abempty_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri