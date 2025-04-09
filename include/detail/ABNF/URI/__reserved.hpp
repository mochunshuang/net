#pragma once

#include "./__gen_delims.hpp"
#include "./__sub_delims.hpp"

namespace mcs::ABNF::URI
{
    // reserved      = gen-delims / sub-delims
    constexpr bool reserved(octet_t c) noexcept
    {
        return gen_delims(c) || sub_delims(c);
    }
}; // namespace mcs::ABNF::URI