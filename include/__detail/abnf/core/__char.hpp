#pragma once

#include "../generate/__range.hpp"

namespace mcs::abnf::core
{
    // CHAR           =  %x01-7F
    using CHAR = generate::Range<0x01, 0x7F>; // NOLINT

}; // namespace mcs::abnf::core