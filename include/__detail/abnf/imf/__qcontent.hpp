#pragma once

#include "./__qtext.hpp"
#include "./__quoted_pair.hpp"

namespace mcs::abnf::imf
{
    //  qcontent        =   qtext / quoted-pair
    using qcontent = alternative<qtext, quoted_pair>;
}; // namespace mcs::abnf::imf