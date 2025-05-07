#pragma once

#include "./__obs_text.hpp"

namespace mcs::abnf::http
{
    /**
     * etagc = "!" / %x23-7E ; '#'-'~'
                   / obs-text
     *
     */
    using etagc = alternative<Char<'!'>, Range<0x23, 0x7E>, obs_text>; // NOLINT
}; // namespace mcs::abnf::http