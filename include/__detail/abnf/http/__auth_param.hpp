#pragma once

#include "./__token.hpp"
#include "./__bws.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::http
{
    // auth-param = token BWS "=" BWS ( token / quoted-string )
    using auth_param =
        sequence<token, BWS, Char<'='>, BWS, alternative<token, quoted_string>>;

}; // namespace mcs::abnf::http