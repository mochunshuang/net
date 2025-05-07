#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    //   pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    using pchar = alternative<unreserved, pct_encoded, sub_delims, Char<':'>, Char<'@'>>;
}; // namespace mcs::abnf::uri