#pragma once

#include "./__segment.hpp"

namespace mcs::abnf::uri
{
    // path-abempty  = *( "/" segment )
    constexpr CheckResult path_abempty(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return Success{0};

        if (k_size > 0 && sp[0] != '/')
            return Fail(0);

        // handle: 1*( "/" segment )
        static_assert(segment(empty_span_param)); // NOTE: so '/' is good case
        size_t index = 1;                         // check_span start index
        while (index < k_size)
        {
            // NOTE: check segment until find '/' or other error char
            const auto k_ret = segment(sp.subspan(index));
            if (k_ret)
            {
                index += k_ret->count;
                continue;
            }

            static_assert(not segment(std::array<OCTET, 1>{'/'}));
            // update index point to the char of ('/' or other error char)
            // NOTE: if chck_span = "//xxx" => index = index + 0;
            // NOTE: if chck_span = "x/" => index = index + 1;
            index = index + k_ret.error().index();
            if (sp[index] == '/')
            {
                ++index;
                continue;
            }
            return Fail(index);
        }
        return Success{k_size};
    }
}; // namespace mcs::abnf::uri