#pragma once

#include "./__content_coding.hpp"

namespace mcs::abnf::http
{
    // codings = content-coding / "identity" / "*"
    using codings = alternative<content_coding, String<"identity">, Char<'*'>>;
}; // namespace mcs::abnf::http