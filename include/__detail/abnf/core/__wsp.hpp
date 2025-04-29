#pragma once

#include "./__sp.hpp"
#include "./__htab.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // WSP            =  SP / HTAB; white space
    using WSP = operators::alternative<SP, HTAB>;
}; // namespace mcs::abnf::core