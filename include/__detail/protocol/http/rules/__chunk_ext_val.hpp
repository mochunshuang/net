#pragma once

#include "./__token.hpp"
#include "./__quoted_string.hpp"

namespace mcs::protocol::http::rules
{
    // chunk-ext-val = token / quoted-string
    using chunk_ext_val = abnf::alternative<token, quoted_string>;
}; // namespace mcs::protocol::http::rules