#pragma once

#include "./__irregular.hpp"
#include "./__regular.hpp"

namespace mcs::abnf::tfil
{
    /**
     * grandfathered = irregular           ; non-redundant tags registered
                     / regular             ; during the RFC 3066 era
     *
     */
    using grandfathered = alternative<irregular, regular>;
}; // namespace mcs::abnf::tfil