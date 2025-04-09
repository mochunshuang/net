#pragma once

#include <cstdint>
#include <span> // 改用 span 替代 string_view

/**
 * @brief [ABNF Core Rules]
 * https://www.rfc-editor.org/rfc/rfc5234.html#:~:text=7%2Dbit%20ASCII.-,B.1.%20%20Core%20Rules,-Certain%20basic%20rules
 *
 */
namespace mcs::ABNF
{

    /**
     * @brief OCTET          =  %x00-FF
     *                          ; 8 bits of data
     */
    using OCTET = std::uint8_t;

    using default_span_t = const std::span<const OCTET> &;
    using octet_t = const OCTET &;
    inline constexpr auto empty_span_octet = std::span<const OCTET>{}; // NOLINT

    // ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z
    constexpr bool ALPHA(octet_t c) noexcept
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
    // BIT            =  "0" / "1"
    constexpr bool BIT(octet_t c) noexcept
    {
        return c == '1' || c == '0';
    }
    // CHAR           =  %x01-7F
    constexpr bool CHAR(octet_t c) noexcept
    {
        return c >= 0X01 && c <= 0X7F; // NOLINT
    }
    // CTL            =  %x00-1F / %x7F
    constexpr bool CTL(octet_t c) noexcept
    {
        return (c >= 0X00 && c <= 0X1F) || c == 0x7F; // NOLINT
    }
    // DIGIT          =  %x30-39
    constexpr bool DIGIT(octet_t c) noexcept
    {
        static_assert('0' == 0x30);    // NOLINT
        return (c >= '0' && c <= '9'); // NOLINT
    }

    // DQUOTE         =  %x22 ;" (Double Quote)
    inline constexpr OCTET DQUOTE = 0x22; // NOLINT

    // HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    constexpr bool HEXDIG(octet_t c) noexcept
    {
        return DIGIT(c) || (c >= 'A' && c <= 'F');
    }

    // HTAB           =  %x09; horizontal tab
    inline constexpr OCTET HTAB = 0x09; // NOLINT

    // LF             =  %x0A
    inline constexpr OCTET LF = 0x0A; // NOLINT

    // CR             =  %x0D
    inline constexpr OCTET CR = 0x0D; // NOLINT

    // CRLF           =  CR LF
    constexpr bool CRLF(default_span_t pair) noexcept
    {
        return pair.size() == 2 && pair[0] == CR && pair[1] == LF;
    }

    // SP             =  %x20
    inline constexpr OCTET SP = 0x20; // NOLINT

    // WSP            =  SP / HTAB; white space
    constexpr bool WSP(octet_t c) noexcept
    {
        return c == SP || c == HTAB;
    }

    /**
     * @brief 避免在邮件头中使用，因为它可能导致解析问题。
     *
     * 在定义新协议时，建议使用更严格的空白规则（如仅 WSP）。
     *
     */
    // LWSP = *(WSP / CRLF WSP)  ;
    constexpr bool LWSP(default_span_t range) noexcept
    {
        std::size_t i = 0;
        const std::size_t k_size = range.size();
        while (i < k_size)
        {
            if (WSP(range[i]))
                ++i;
            else if (i + 2 < k_size && CRLF(range.subspan(i, 2)) && WSP(range[i + 2]))
                i += 3; // 合并检查 CRLF + WSP
            else
                return false;
        }
        return true;
    }

    //  VCHAR          =  %x21-7E; visible (printing) characters
    constexpr bool VCHAR(octet_t c) noexcept
    {
        static_assert('!' == 0x21 && '~' == 0x7E); // NOLINT
        return c >= '!' && c <= '~';               // NOLINT
    }

}; // namespace mcs::ABNF