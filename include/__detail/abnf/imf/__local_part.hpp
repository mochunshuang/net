#pragma once

#include "./__dot_atom.hpp"
#include "./__obs_local_part.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::imf
{
    // local-part      =   dot-atom / quoted-string / obs-local-part
    using local_part = alternative<dot_atom, quoted_string, obs_local_part>;
}; // namespace mcs::abnf::imf