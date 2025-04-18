#pragma once

#include "./__token.hpp"
#include "./__ows.hpp"
#include "./__quoted_pair.hpp"
#include "./__tchar.hpp"
#include "./__qdtext.hpp"
#include <cassert>

namespace mcs::abnf::http
{

    // quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
    // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
    // token = 1*tchar
    // OWS = *( SP / HTAB )
    // BWS = OWS
    // token = 1*tchar
    // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
    //
    // transfer-coding = token *( OWS ";" OWS transfer-parameter )
    constexpr abnf_result auto transfer_coding(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return simple_result::fail(0);

        const auto k_ret = token(sp);
        if (k_ret) // only token
            return simple_result::success();

        // NOTE: can`t simple split by ';' or by DQUOTE
        static_assert(qdtext(';') && DQUOTE == '"');
        static_assert(not qdtext(DQUOTE) && not tchar(DQUOTE));
        static_assert(VCHAR(DQUOTE) && not tchar(DQUOTE));

        // check *( OWS ";" OWS transfer-parameter )
        std::size_t index = k_ret.error().index();
        static_assert(not tchar(';'));
        while (index < k_size)
        {
            auto ret = OWS(sp.subspan(index));
            if (ret) // NOTE: 1、OWS part. must error
                break;
            index += ret.error().index();

            if (sp[index] != ';') // NOTE: 2、";" part
                return simple_result::fail(index);

            ret = OWS(sp.subspan(index + 1)); // NOTE: 3、OWS part
            if (ret)
                break;
            index = index + 1 + ret.error().index();

            // NOTE: token is no sp or HTAB
            //  OWS = *( SP / HTAB )
            static_assert(not tchar(SP) && not tchar(HTAB));

            // NOTE: 4、transfer-parameter part
            // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
            {
                auto i = index;
                while (i < k_size) // token of transfer-parameter
                {
                    if (tchar(sp[i]))
                    {
                        ++i;
                        continue;
                    }
                    break;
                }
                if (i == index) // NOTE: token count == 0
                    return simple_result::fail(i);

                while (i < k_size) // BWS
                {
                    if (sp[i] == SP || sp[i] == HTAB)
                    {
                        ++i;
                        continue;
                    }
                    break;
                }
                if (sp[i] != '=') // NOTE: it is not a SP or HTAB mush be a char 0f '='
                    return simple_result::fail(i);

                // check: BWS ( token / quoted-string )
                i += 1;
                while (i < k_size) // BWS
                {
                    if (sp[i] == SP || sp[i] == HTAB)
                    {
                        ++i;
                        continue;
                    }
                    break;
                }

                // NOTE: can distinguish token and quoted-string by DQUOTE
                static_assert(not tchar(DQUOTE));

                // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
                if (sp[i] == DQUOTE) // NOTE: match quoted-string
                {
                    // find second  DQUOTE
                    auto begin = i++;
                    while (i < k_size)
                    {
                        // NOTE: quoted-pair include DQUOTE: "\DQUOTE" is not find DQUOTE
                        if (sp[i] == DQUOTE && sp[i - 1] != '\\')
                            break;
                        ++i;
                    }
                    if (i == k_size) // not find second  DQUOTE
                        return simple_result::fail(begin);

                    // check (begin,end)
                    assert(sp[i] == DQUOTE);
                    auto end = i;
                    for (auto j = begin + 1; j < end;)
                    {
                        if (qdtext(sp[j]))
                        {
                            ++j;
                            continue;
                        }
                        if (j + 1 < end && quoted_pair(sp[j], sp[j + 1]))
                        {
                            j += 2;
                            continue;
                        }
                        return simple_result::fail(i);
                    }
                    // update index
                    index = end + 1;
                    continue;
                }

                // NOTE: match token
                // token = 1*tchar
                auto token_start = i;
                while (i < k_size) // token of transfer-parameter
                {
                    if (tchar(sp[i]))
                    {
                        ++i;
                        continue;
                    }
                    break;
                }
                if (i == token_start) // NOTE: no token in tail of transfer-parameter
                    return simple_result::fail(i);
                // update index
                index = i;
            }
        }
        return index == k_size ? simple_result::success() : simple_result::fail(index);
    }
}; // namespace mcs::abnf::http