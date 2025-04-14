#pragma once

#include "./__userinfo.hpp"
#include "./__host.hpp"
#include "./__port.hpp"

namespace mcs::abnf::uri
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    constexpr bool authority(span_param_in sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return false;

        size_t idx_0 = k_size;
        for (size_t i = 0; i < k_size; ++i)
        {
            if (sp[i] == '@')
            {
                idx_0 = i;
                break;
            }
        }
        // first check: userinfo if find '@'
        if (idx_0 < k_size)
        {
            const auto k_u = sp.first(idx_0);
            if (not userinfo(k_u))
                return false;

            // k_remain is host [ ":" port ]
            const auto k_remain = sp.subspan(idx_0 + 1);
            if (host(k_remain))
                return true;
            // match host ":" port
            size_t idx_1 = k_size;
            for (size_t left = idx_0 + 1, right = k_size; right-- > left;)
            {
                if (sp[right] == ':')
                {
                    idx_1 = right;
                    break;
                }
            }
            if (idx_1 == k_size)
                return false;
            const auto k_h = sp.subspan(idx_0 + 1, idx_1 - idx_0 - 1);
            const auto k_p = sp.subspan(idx_1 + 1);
            return host(k_h) && port(k_p);
        }

        // not find '@' then sp is host [ ":" port ]
        if (host(sp))
            return true;
        // match host ":" port
        size_t idx_1 = k_size;
        for (size_t left = 0, right = k_size; right-- > left;)
        {
            if (sp[right] == ':')
            {
                idx_1 = right;
                break;
            }
        }
        if (idx_1 == k_size)
            return false;
        const auto k_h = sp.first(idx_1);
        const auto k_p = sp.subspan(idx_1 + 1);
        return host(k_h) && port(k_p);
    }

}; // namespace mcs::abnf::uri