#pragma once

#include "./__segment_nz_nc.hpp"
#include "./__path_abempty.hpp"

namespace mcs::abnf::uri
{
    // path-noscheme = segment-nz-nc *( "/" segment )
    constexpr CheckResult path_noscheme(default_span_t sp) noexcept
    {
        static_assert(not segment_nz_nc(empty_span)); // NOTE: so need sp.empty() is false

        const auto k_size = sp.size();
        if (k_size == 0)
            return Fail(0);

        // NOTE: '/' is not segment_nz then can split
        static_assert(not segment_nz_nc(std::array<OCTET, 1>{'/'}));

        // handle: segment_nz_nc
        const auto k_first = std::ranges::find(sp, '/');
        if (k_first == sp.end())
            return segment_nz_nc(
                sp); // NOTE: return early when sp is only have segment_nz_nc

        // split to two-part and handle respectively
        // handle: segment-nz
        size_t d = std::distance(sp.begin(), k_first);
        if (const auto k_ret = segment_nz_nc(sp.first(d)); not k_ret)
            return k_ret;

        // handle: 1*( "/" segment )
        // NOTE: path-abempty  = *( "/" segment )
        if (const auto k_ret = path_abempty(sp.subspan(d)); not k_ret)
            return Fail(d + k_ret.error().index());
        return Success{k_size};
    }
}; // namespace mcs::abnf::uri
