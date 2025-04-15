#pragma once

#include "./__authority.hpp"
#include "./__path_abempty.hpp"
#include "./__path_absolute.hpp"
#include "./__path_rootless.hpp"
#include "./__path_empty.hpp"

namespace mcs::rfc::uri
{
    struct authority
    {
        // authority     = [ userinfo "@" ] host [ ":" port ]
        // 3 个成员即可。 没有 其他类型的可能
    };
    constexpr ReturnType<authority> make_authority(span_param_in sp) noexcept
    {
        if (parse::authority(sp))
            return authority{};
        return err_index(0);
    }
}; // namespace mcs::rfc::uri
