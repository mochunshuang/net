#pragma once

#include "./__token.hpp"
#include "./__quoted_string.hpp"

namespace mcs::abnf::http
{
    // parameter-value = ( token / quoted-string )
    using parameter_value = alternative<token, quoted_string>;

}; // namespace mcs::abnf::http