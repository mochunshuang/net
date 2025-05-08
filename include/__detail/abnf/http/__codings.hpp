#pragma once

#include "./__content_coding.hpp"

namespace mcs::abnf::http
{
    // codings = content-coding / "identity" / "*"
    using codings = alternative<String<"identity">, Char<'*'>, content_coding>;
}; // namespace mcs::abnf::http