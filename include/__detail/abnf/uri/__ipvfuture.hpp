#pragma once

#include "./__unreserved.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using IPvFuture_rule =
            sequence<Char<'v'>, one_or_more<HEXDIG>, Char<'.'>,
                     one_or_more<alternative<unreserved, sub_delims, Char<':'>>>>;
    };
    // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
    struct IPvFuture : SimpleRule<IPvFuture, rules::IPvFuture_rule>
    {
        using SimpleRule<IPvFuture, rules::IPvFuture_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri