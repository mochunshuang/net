#pragma once

#include "./__path_rootless.hpp"
#include <expected>

namespace mcs::abnf::uri
{
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    constexpr CheckResult path_absolute(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0 || sp[0] != '/')
            return Fail(0);

        // NOTE: path-rootless = segment-nz *( "/" segment )
        // ["a"] => *1"a" so path_rootless is one or none
        if (k_size == 1)
            return Success{k_size};
        const auto k_ret = path_rootless(sp.subspan(1));
        if (not k_ret)
            return Fail(1 + k_ret.error().index());
        return Success{1 + k_ret->count};
    }
}; // namespace mcs::abnf::uri