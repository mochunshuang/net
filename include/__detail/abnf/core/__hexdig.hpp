#pragma once

#include "./__digit.hpp"
#include "../generate/__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    using HEXDIG = operators::alternative<
        DIGIT, generate::CharInsensitive<'A'>, generate::CharInsensitive<'B'>,
        generate::CharInsensitive<'C'>, generate::CharInsensitive<'D'>,
        generate::CharInsensitive<'E'>, generate::CharInsensitive<'F'>>;
}; // namespace mcs::abnf::core