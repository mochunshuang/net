#pragma once

#include "../tool/__safe_subspan.hpp"
#include <algorithm>

namespace mcs::ABNF::URI
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    constexpr bool authority(default_span_t range) noexcept
    {
        return true;
    }

}; // namespace mcs::ABNF::URI