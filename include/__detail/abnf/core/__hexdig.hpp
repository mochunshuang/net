#pragma once

#include "./__digit.hpp"
#include "../generate/__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    using HEXDIG = operators::alternative<DIGIT, generate::Char<'A'>, generate::Char<'B'>,
                                          generate::Char<'C'>, generate::Char<'D'>,
                                          generate::Char<'E'>, generate::Char<'F'>>;
    inline constexpr HEXDIG hexdig{}; // NOLINT
}; // namespace mcs::abnf::core