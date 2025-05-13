#pragma once

#include "../../../abnf/http/__quoted_string.hpp"

namespace mcs::protocol::http::rules
{
    // quoted-string = <quoted-string, see [HTTP], Section 5.6.4>
    using quoted_string = ::mcs::abnf::http::quoted_string;
}; // namespace mcs::protocol::http::rules