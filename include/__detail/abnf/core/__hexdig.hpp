#pragma once

#include "./__digit.hpp"
#include "../generate/__char.hpp"
#include "../operators/__alternative.hpp"

namespace mcs::abnf::core
{
    // HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    using HEXDIG = operators::alternative<
        DIGIT, generate::InsensitiveChar<'A'>, generate::InsensitiveChar<'B'>,
        generate::InsensitiveChar<'C'>, generate::InsensitiveChar<'D'>,
        generate::InsensitiveChar<'E'>, generate::InsensitiveChar<'F'>>;
    inline constexpr HEXDIG hexdig{}; // NOLINT
}; // namespace mcs::abnf::core