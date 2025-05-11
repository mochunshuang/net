#pragma once

#include "./__token.hpp"
#include "./__quoted_string.hpp"
#include "./__bws.hpp"

namespace mcs::abnf::http
{
    // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
    using transfer_parameter = sequence<assert_not_string<"q", token>, BWS, Char<'='>,
                                        BWS, alternative<token, quoted_string>>;
}; // namespace mcs::abnf::http