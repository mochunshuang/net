#pragma once

#include "./__pchar.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using query_rule =
            zero_or_more<alternative<pchar, CharSensitive<'/'>, CharSensitive<'?'>>>;
    };
    // query         = *( pchar / "/" / "?" )
    struct query : SimpleRule<query, rules::query_rule>
    {
    };

}; // namespace mcs::abnf::uri