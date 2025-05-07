#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using fragment_rule = zero_or_more<alternative<pchar, Char<'/'>, Char<'?'>>>;
    };
    // fragment      = *( pchar / "/" / "?" )
    struct fragment : SimpleRule<fragment, rules::fragment_rule>
    {
        using SimpleRule<fragment, rules::fragment_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri