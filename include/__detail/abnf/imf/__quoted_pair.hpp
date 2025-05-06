#pragma once

#include "./__obs_qp.hpp"

namespace mcs::abnf::imf
{
    // quoted-pair     =   ("\" (VCHAR / WSP)) / obs-qp
    using quoted_pair =
        alternative<sequence<CharSensitive<'\\'>, alternative<VCHAR, WSP>>, obs_qp>;
}; // namespace mcs::abnf::imf