#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    /*
    other-range = 1*( %x21-2B ; '!'-'+'
    / %x2D-7E ; '-'-'~'
    )
    */
    using other_range =
        one_or_more<alternative<Range<0x21, 0x2B>, Range<0x2D, 0x7E>>>; // NOLINT
}; // namespace mcs::abnf::http