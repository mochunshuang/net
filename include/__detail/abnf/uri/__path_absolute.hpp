#pragma once

#include "./__path_rootless.hpp"
#include <expected>

namespace mcs::abnf::uri
{
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    constexpr auto path_absolute(span_param_in sp) noexcept -> abnf_result auto
    {
        using builder = result_builder<result<1>>;
        const auto k_size = sp.size();
        if (k_size == 0 || sp[0] != '/')
            return builder::fail(0);

        // NOTE: path-rootless = segment-nz *( "/" segment )
        // ["a"] => *1"a" so path_rootless is one or none
        if (k_size == 1)
            return builder::success(span{.start = 0, .count = k_size});
        const auto k_ret = path_rootless(sp.subspan(1));
        if (not k_ret)
            return builder::fail(1 + k_ret.error().index());
        return builder::success(span{.start = 0, .count = k_size});
    }
}; // namespace mcs::abnf::uri