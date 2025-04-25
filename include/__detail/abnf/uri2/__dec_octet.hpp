#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    /**
     * @brief
     * dec-octet = DIGIT                 ; 0-9
     *           / %x31-39 DIGIT         ; 10-99
     *           / "1" 2DIGIT            ; 100-199
     *           / "2" %x30-34 DIGIT     ; 200-249
     *           / "25" %x30-35          ; 250-255
     */
    using dec_octet = alternative<DIGIT,sequence<Range<0x31,0x39>>>;

}; // namespace mcs::abnf::uri
