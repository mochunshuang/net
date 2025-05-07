#pragma once

#include "./__extlang.hpp"

namespace mcs::abnf::tfil
{
    /**
     * language   = 2*3ALPHA            ; shortest ISO 639 code
                    ["-" extlang]       ; sometimes followed by
                                     ; extended language subtags
                    / 4ALPHA              ; or reserved for future use
                    / 5*8ALPHA            ; or registered language subtag
     *
     */
    using language =
        sequence<repetition<2, 3, ALPHA>, optional<sequence<Char<'-'>, extlang>>,
                 times<4, ALPHA>, repetition<5, 8, ALPHA>>; // NOLINT
}; // namespace mcs::abnf::tfil