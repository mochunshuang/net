#pragma once

#include "./__userinfo.hpp"
#include "./__host.hpp"
#include "./__port.hpp"

#include "../tool/__split_span.hpp"

#include <cstddef>

namespace mcs::ABNF::URI
{
    // authority     = [ userinfo "@" ] host [ ":" port ]
    constexpr bool authority(default_span_t sp) noexcept
    {
        const auto k_size = sp.size();
        if (k_size == 0)
            return false;

        size_t idx0 = k_size;
        size_t idx1 = k_size;
        for (size_t left = 0, right = k_size - 1; left < right; ++left, --right)
        {
            if (idx0 == k_size && sp[left] == '@')
                idx0 = left;
            if (idx1 == k_size && sp[right] == ':')
                idx1 = right;

            if (idx1 != k_size && idx0 != k_size)
                break;
        }

        if (idx0 == k_size && idx1 == k_size) // only host
            return host(sp);
        if (idx0 != k_size && idx1 == k_size) // has userinfo but has no port
        {
            const auto [u, h] = tool::split_span_first(sp, '@');
            return userinfo(u) && host(h);
        }
        if (idx0 == k_size && idx1 != k_size) // has no userinfo but has port
        {
            const auto [h, p] = tool::split_span_last(sp, ':');
            return host(h) && port(p);
        }
        const auto [u, remain] = tool::split_span_first(sp, '@');
        const auto [h, p] = tool::split_span_last(remain, ':');
        return userinfo(u) && host(h) && port(p);
    }

}; // namespace mcs::ABNF::URI