#pragma once

#include "../generate/__range.hpp"

namespace mcs::abnf::core
{
    //  VCHAR          =  %x21-7E; visible (printing) characters
    using VCHAR = generate::Range<0x21, 0x7F>; // NOLINT

}; // namespace mcs::abnf::core