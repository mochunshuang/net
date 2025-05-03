#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using scheme_rule =
            sequence<ALPHA,
                     zero_or_more<alternative<ALPHA, DIGIT, any_of<'+', '-', '.'>>>>;
    };
    // scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
    struct scheme : SimpleRule<scheme, rules::scheme_rule>
    {
                using SimpleRule<scheme, rules::scheme_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri