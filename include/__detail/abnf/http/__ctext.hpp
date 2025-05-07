#pragma once

#include "./__obs_text.hpp"

namespace mcs::abnf::http
{
    /**
     * ctext = HTAB / SP / %x21-27 ; '!'-'''
            / %x2A-5B ; '*'-'['
            / %x5D-7E ; ']'-'~'
            / obs-text
     *
     */
    using ctext = alternative<HTAB, SP, Range<0x21, 0x27>, Range<0x2A, 0x5B>, // NOLINT
                              Range<0x5D, 0x7E>, obs_text>;                   // NOLINT
}; // namespace mcs::abnf::http