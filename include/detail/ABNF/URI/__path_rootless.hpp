#pragma once

#include "./__segment_nz.hpp"
#include "./__segment.hpp"
#include <array>

namespace mcs::ABNF::URI
{
    // path-rootless = segment-nz *( "/" segment )
    constexpr CheckResult path_rootless(default_span_t sp) noexcept
    {
        using span_t = std::decay_t<default_span_t>;
        static_assert(not segment_nz(span_t{})); // NOTE: so need sp.empty() is false

        const auto k_size = sp.size();
        if (k_size == 0)
            return std::unexpected{Info{0}};

        // NOTE: '/' is not segment_nz then can split
        static_assert(not segment_nz(std::array<OCTET, 1>{'/'}));

        // handle: segment-nz
        auto first = std::ranges::find(sp, '/');
        if (first == sp.end())
            return segment_nz(sp); // NOTE: return early when sp is only have segment-nz

        // split to two-part and handle respectively
        // handle: segment-nz
        size_t d = std::distance(sp.begin(), first);
        auto ret = segment_nz(sp.first(d));
        if (not ret)
            return ret;

        // handle: 1*( "/" segment )
        static_assert(segment(span_t{})); // NOTE: so '/' is good case
        auto index = d + 1;               // check_span start index
        while (index < k_size)
        {
            // NOTE: check segment until find '/' or other error char
            auto ret = segment(sp.subspan(index));
            if (ret)
            {
                index += ret->count;
                continue;
            }

            static_assert(not segment(std::array<OCTET, 1>{'/'}));
            // update index point to the char of ('/' or other error char)
            // NOTE: if chck_span = "//xxx" => index = index + 0;
            // NOTE: if chck_span = "x/" => index = index + 1;
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
