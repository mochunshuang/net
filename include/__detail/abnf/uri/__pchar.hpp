#pragma once

#include "./__unreserved.hpp"
#include "./__pct_encoded.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
    constexpr bool pchar(octet_param_in v) noexcept
    {
        return unreserved(v) || sub_delims(v) || (v == ':' || v == '@');
    }
    constexpr bool pchar(octet_param_in a, octet_param_in b, octet_param_in c) noexcept
    {
        // NOTE: pchar Can be overloaded Because the following assertion holds true
        static_assert(not unreserved('%') && not sub_delims('%'));
        return pct_encoded(a, b, c); // pct-encoded   = "%" HEXDIG HEXDIG
    }
}; // namespace mcs::abnf::uri