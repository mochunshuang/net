#pragma once
#include "./__atom.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::imf
{
    // word            =   atom / quoted-string
    using word = alternative<atom, quoted_string>;
}; // namespace mcs::abnf::imf