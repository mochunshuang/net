#pragma once

#include "./__obs_text.hpp"

namespace mcs::abnf::http
{
    /**
    qdtext = HTAB / SP / "!" / %x23-5B ; '#'-'['
           / %x5D-7E ; ']'-'~'
           / obs-text
    */
    using qdtext = alternative<HTAB, SP, Char<'!'>, Range<0x23, 0x5B>, // NOLINT
                               Range<0x5D, 0x7E>, obs_text>;           // NOLINT
}; // namespace mcs::abnf::http