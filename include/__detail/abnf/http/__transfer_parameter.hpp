#pragma once

#include "./__token.hpp"
#include "./__tchar.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::http
{
    /**
    qdtext = HTAB / SP / "!" / %x23-5B ; '#'-'['
           / %x5D-7E ; ']'-'~'
           / obs-text
    */
    // obs-text = %x80-FF
    // quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
    // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
    // token = 1*tchar
    // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
    constexpr abnf_result auto transfer_parameter(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);
        std::size_t index = 0;
        while (index < k_size) // token
        {
            if (tchar(sp[index]))
            {
                ++index;
                continue;
            }
            break;
        }
        if (index == 0)
            return simple_result::fail(index);

        static_assert(not tchar(SP) && not tchar(HTAB) && not tchar('='));
        while (index < k_size) // BWS
        {
            if (sp[index] == SP || sp[index] == HTAB)
            {
                ++index;
                continue;
            }
            break;
        }

        static_assert('=' != SP && '=' != HTAB);
        if (sp[index] != '=') //  '='
            return simple_result::fail(index);

        // check: BWS ( token / quoted-string )
        index += 1;
        while (index < k_size) // BWS
        {
            if (sp[index] == SP || sp[index] == HTAB)
            {
                ++index;
                continue;
            }
            break;
        }
        const auto k_sub = sp.subspan(index);
        return (token(k_sub) || quoted_string(k_sub)) ? simple_result::success()
                                                      : simple_result::fail(index);
    }
}; // namespace mcs::abnf::http