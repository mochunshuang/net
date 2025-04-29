#pragma once

#include "../generate/__range.hpp"

namespace mcs::abnf::core
{
    // DIGIT          =  %x30-39
    using DIGIT = generate::Range<0x30, 0x39>; // NOLINT

}; // namespace mcs::abnf::core