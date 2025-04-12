#pragma once

#include "./__authority.hpp"
#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"

namespace mcs::ABNF::URI
{
    /**
     * @brief
     *  hier-part     = "//" authority path-abempty
                 / path-absolute
                 / path-rootless
                 / path-empty
     */
    constexpr bool hier_part(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size > 2 && sp[0] == '/' && sp[1] == '/')
        {
            // authority     = [ userinfo "@" ] host [ ":" port ]
            // authority need no '/
            static_assert(not unreserved('/'));
            //  path-abempty  = *( "/" segment )
            //  segment       = *pchar
            //  pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
            static_assert(not pchar('/'));
            size_t index = k_size;
            constexpr size_t k_start_index = 2;
            for (size_t i = k_start_index; i < k_size; ++i)
            {
                if (sp[i] == '/')
                {
                    index = i;
                    break;
                }
            }
            if (index == k_size)
            {
                if (authority(sp.subspan(k_start_index, index - k_start_index)))
                    return true;
            }
            else
            {
                if (authority(sp.subspan(k_start_index, index - k_start_index)) &&
                    path_abempty(sp.subspan(index)))
                    return true;
            }
        }
        return path_absolute(sp) || path_rootless(sp) || path_empty(sp);
    }
}; // namespace mcs::ABNF::URI