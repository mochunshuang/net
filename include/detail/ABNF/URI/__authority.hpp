#pragma once

#include "./__userinfo.hpp"
#include "./__host.hpp"
#include "./__port.hpp"

namespace mcs::ABNF::URI
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    constexpr bool authority(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return false;

        size_t idx_0 = k_size;
        size_t idx_1 = k_size;
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == '@')
            {
                idx_0 = i;
                break;
            }
        }
        for (size_t left = (idx_0 < k_size ? idx_0 + 1 : 0), right = k_size;
             right-- > left;)
        {
            if (sp[right] == ':')
            {
                idx_1 = right;
                break;
            }
        }

        if (idx_0 == k_size && idx_1 == k_size) // only host
            return host(sp);
        if (idx_0 < k_size && idx_1 == k_size) // has userinfo but has no port
        {
            const auto k_u = sp.first(idx_0);
            const auto k_h = sp.subspan(idx_0 + 1);
            return userinfo(k_u) && host(k_h);
        }
        if (idx_0 == k_size && idx_1 < k_size) // has no userinfo but has port
        {
            const auto k_u = sp.first(idx_1);
            const auto k_p = sp.subspan(idx_1 + 1);
            return host(k_u) && port(k_p);
        }
        const auto k_u = sp.first(idx_0);
        const auto k_h = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
        const auto k_p = sp.subspan(idx_1 + 1);
        return userinfo(k_u) && host(k_h) && port(k_p);
    }

}; // namespace mcs::ABNF::URI