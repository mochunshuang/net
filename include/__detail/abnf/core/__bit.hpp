#pragma once

#include "../generate/__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // BIT            =  "0" / "1"
    using BIT = operators::alternative<generate::Char<'0'>, generate::Char<'1'>>;

}; // namespace mcs::abnf::core