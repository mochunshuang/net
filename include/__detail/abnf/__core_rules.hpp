#pragma once

#include "./__core_types.hpp"
#include "./__core_concepts.hpp"

/**
 * @brief [ABNF Core Rules]
 * https://www.rfc-editor.org/rfc/rfc5234.html#:~:text=7%2Dbit%20ASCII.-,B.1.%20%20Core%20Rules,-Certain%20basic%20rules
 *
 */
namespace mcs::abnf
{
    // ALPHA          =  %x41-5A / %x61-7A   ; A-Z / a-z
    constexpr abnf_result auto ALPHA(octet_param_in c) noexcept
    {
        return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
    }
    // BIT            =  "0" / "1"
    constexpr abnf_result auto BIT(octet_param_in c) noexcept
    {
        return c == '1' || c == '0';
    }
    // CHAR           =  %x01-7F
    constexpr abnf_result auto CHAR(octet_param_in c) noexcept
    {
        return c >= 0X01 && c <= 0X7F; // NOLINT
    }
    // CTL            =  %x00-1F / %x7F
    constexpr abnf_result auto CTL(octet_param_in c) noexcept
    {
        return c <= 0X1F || c == 0x7F; // NOLINT
    }
    // DIGIT          =  %x30-39
    constexpr abnf_result auto DIGIT(octet_param_in c) noexcept
    {
        static_assert('0' == 0x30);    // NOLINT
        return (c >= '0' && c <= '9'); // NOLINT
    }

    // DQUOTE         =  %x22 ;" (Double Quote)
    inline constexpr OCTET DQUOTE = 0x22; // NOLINT

    // NOTE: 不区分大小写除非特别说明
    //  HEXDIG         =  DIGIT / "A" / "B" / "C" / "D" / "E" / "F"
    constexpr abnf_result auto HEXDIG(octet_param_in c) noexcept
    {
        return DIGIT(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
    }

    // HTAB           =  %x09; horizontal tab
    inline constexpr OCTET HTAB = 0x09; // NOLINT

    // LF             =  %x0A
    inline constexpr OCTET LF = 0x0A; // NOLINT

    // CR             =  %x0D
    inline constexpr OCTET CR = 0x0D; // NOLINT

    // CRLF           =  CR LF
    constexpr abnf_result auto CRLF(octet_param_in a, octet_param_in b) noexcept
    {
        return a == CR && b == LF;
    }
    constexpr abnf_result auto CRLF(span_param_in sp) noexcept
    {
        if (sp.size() != 2)
            return false;
        return CRLF(sp[0], sp[1]);
    }

    // SP             =  %x20
    inline constexpr OCTET SP = 0x20; // NOLINT

    // WSP            =  SP / HTAB; white space
    constexpr abnf_result auto WSP(octet_param_in c) noexcept
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
    constexpr abnf_result auto LWSP(span_param_in range) noexcept
    {
        std::size_t i = 0;
        const std::size_t k_size = range.size();
        while (i < k_size)
        {
            if (WSP(range[i]))
                ++i;
            else
            {
                if (range[i] == CR && i + 2 < k_size)
                {
                    if (range[i + 1] == LF && WSP(range[i + 2]))
                    {
                        i += 3;
                        continue;
                    }
                    return false;
                }
                return false;
            }
        }
        return true;
    }

    //  VCHAR          =  %x21-7E; visible (printing) characters
    constexpr abnf_result auto VCHAR(octet_param_in c) noexcept
    {
        static_assert('!' == 0x21 && '~' == 0x7E); // NOLINT
        return c >= '!' && c <= '~';               // NOLINT
    }

}; // namespace mcs::abnf