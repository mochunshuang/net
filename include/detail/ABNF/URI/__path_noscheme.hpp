#pragma once

#include "./__segment_nz_nc.hpp"
#include "./__segment.hpp"

namespace mcs::ABNF::URI
{
    // path-noscheme = segment-nz-nc *( "/" segment )
    constexpr CheckResult path_noscheme(default_span_t sp) noexcept
    {
        using span_t = std::decay_t<default_span_t>;
        static_assert(not segment_nz_nc(span_t{})); // NOTE: so need sp.empty() is false

        const auto k_size = sp.size();
        if (k_size == 0)
            return std::unexpected{Info{0}};

        // NOTE: '/' is not segment_nz then can split
        static_assert(not segment_nz_nc(std::array<OCTET, 1>{'/'}));

        // handle: segment_nz_nc
        auto first = std::ranges::find(sp, '/');
        if (first == sp.end())
            return segment_nz_nc(
                sp); // NOTE: return early when sp is only have segment_nz_nc

        // split to two-part and handle respectively
        // handle: segment-nz
        size_t d = std::distance(sp.begin(), first);
        auto ret = segment_nz_nc(sp.first(d));
        if (not ret)
            return ret;

        // handle: 1*( "/" segment )
        static_assert(segment(span_t{})); // NOTE: so '/' is good case
        auto index = d + 1;               // check_span start index
        while (index < k_size)
        {
            auto ret = segment(sp.subspan(index));
            if (ret)
            {
                index += ret->count;
                continue;
            }
            static_assert(not segment(std::array<OCTET, 1>{'/'}));
            index = index + ret.error().index();
            if (sp[index] == '/')
            {
                ++index;
                continue;
            }
            return std::unexpected(Info(index));
        }
        return Success{k_size};
    }
}; // namespace mcs::ABNF::URI
