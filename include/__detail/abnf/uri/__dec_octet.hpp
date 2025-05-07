#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::uri
{
    // ABNF 的书写顺序看似自上而下，但实际解析需遵循 最长匹配优先（Longest Match First）
    /**
     * @brief
     * dec-octet = DIGIT                 ; 0-9
     *           / %x31-39 DIGIT         ; 10-99
     *           / "1" 2DIGIT            ; 100-199
     *           / "2" %x30-34 DIGIT     ; 200-249
     *           / "25" %x30-35          ; 250-255
     */
    using dec_octet = alternative< // 优先尝试最长规则 //NOTE: 优先处理特殊
        sequence<Char<'2'>, Char<'5'>, Range<0x30, 0x35>>, // NOLINT
        sequence<Char<'2'>, Range<0x30, 0x34>, DIGIT>,     // NOLINT
        sequence<Char<'1'>, times<2, DIGIT>>,              // NOLINT
        sequence<Range<0x31, 0x39>, DIGIT>,                // NOLINT
        DIGIT                                              // NOLINT
        >;

}; // namespace mcs::abnf::uri
