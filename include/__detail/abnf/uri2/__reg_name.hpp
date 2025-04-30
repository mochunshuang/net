#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using reg_name_rule =
            zero_or_more<alternative<unreserved, pct_encoded, sub_delims>>;
    };
    //  reg-name      = *( unreserved / pct-encoded / sub-delims )
    struct reg_name : SimpleRule<reg_name, rules::reg_name_rule>
    {
    };

}; // namespace mcs::abnf::uri