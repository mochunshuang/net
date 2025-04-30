#pragma once

#include "./__unreserved.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using IPvFuture_rule = sequence<
            CharSensitive<'v'>, one_or_more<HEXDIG>, CharSensitive<'.'>,
            one_or_more<alternative<unreserved, sub_delims, CharSensitive<':'>>>>;
    };
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    struct IPvFuture : SimpleRule<IPvFuture, rules::IPvFuture_rule>
    {
    };

}; // namespace mcs::abnf::uri