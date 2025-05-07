#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::tfil
{
    /**
     *singleton     = DIGIT               ; 0 - 9
                    / %x41-57             ; A - W
                    / %x59-5A             ; Y - Z
                    / %x61-77             ; a - w
                    / %x79-7A             ; y - z
     */
    using singleton = alternative<DIGIT, Range<'A', 0x57>, Range<0x59, 0x5A>, // NOLINT
                                  Range<0x61, 0x77>, Range<0x79, 0x7A>>;      // NOLINT
}; // namespace mcs::abnf::tfil