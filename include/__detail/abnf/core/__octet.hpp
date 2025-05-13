#pragma once

#include "../generate/__range.hpp"

namespace mcs::abnf::core
{
    // OCTET          =  %x00-FF ; 8 bits of data
    using OCTET = generate::Range<0x00, 0xFF>; // NOLINT

}; // namespace mcs::abnf::core