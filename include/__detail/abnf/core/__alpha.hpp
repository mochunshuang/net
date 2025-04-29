#pragma once

#include "../generate/__range.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z
    using ALPHA =
        operators::alternative<generate::Range<'A', 'Z'>, generate::Range<'a', 'z'>>;

}; // namespace mcs::abnf::core