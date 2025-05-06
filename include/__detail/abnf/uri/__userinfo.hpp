#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using userinfo_rule = zero_or_more<
            alternative<unreserved, pct_encoded, sub_delims, CharSensitive<':'>>>;
    };
    // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
    struct userinfo : SimpleRule<userinfo, rules::userinfo_rule>
    {
        using SimpleRule<userinfo, rules::userinfo_rule>::SimpleRule;
    };

}; // namespace mcs::abnf::uri