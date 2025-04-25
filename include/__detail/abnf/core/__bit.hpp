#pragma once

#include "../generate/__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // BIT            =  "0" / "1"
    using BIT = operators::alternative<generate::SensitiveChar<'0'>,
                                       generate::SensitiveChar<'1'>>;
    inline constexpr BIT bit{}; // NOLINT

}; // namespace mcs::abnf::core