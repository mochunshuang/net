#pragma once

#include "./__token.hpp"
#include "./__parameters.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::http
{
    // expectation = token [ "=" ( token / quoted-string ) parameters ]
    using expectation = sequence<
        token,
        optional<sequence<Char<'='>, alternative<token, quoted_string>, parameters>>>;
}; // namespace mcs::abnf::http