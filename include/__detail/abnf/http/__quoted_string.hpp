#pragma once

#include "./__qdtext.hpp"
#include "./__quoted_pair.hpp"

namespace mcs::abnf::http
{
    // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
    using quoted_string =
        sequence<DQUOTE, zero_or_more<alternative<qdtext, quoted_pair>>, DQUOTE>;
}; // namespace mcs::abnf::http