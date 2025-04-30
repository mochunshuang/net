#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    namespace rules
    {
        using port_rule = zero_or_more<DIGIT>;
    };
    // port          = *DIGIT
    struct port : SimpleRule<port, rules::port_rule>
    {
    };

}; // namespace mcs::abnf::uri