#pragma once

#include "./__gen_delims.hpp"
#include "./__sub_delims.hpp"

namespace mcs::abnf::uri
{
    // reserved      = gen-delims / sub-delims
    constexpr bool reserved(octet_param_in c) noexcept
    {
        return gen_delims(c) || sub_delims(c);
    }
}; // namespace mcs::abnf::uri