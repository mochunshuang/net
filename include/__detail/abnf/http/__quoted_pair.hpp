#pragma once

#include "./__obs_text.hpp"

namespace mcs::abnf::http
{
    // quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
    using quoted_pair = sequence<Char<'\\'>, alternative<HTAB, SP, VCHAR, obs_text>>;
}; // namespace mcs::abnf::http