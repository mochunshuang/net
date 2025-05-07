#pragma once

#include "../generate/__char.hpp"
#include "../generate/__range.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // CTL            =  %x00-1F / %x7F ;
    using CTL = operators::alternative<generate::Range<0x00, 0x1F>, // NOLINT
                                       generate::Char<0x7F>>;       // NOLINT

}; // namespace mcs::abnf::core