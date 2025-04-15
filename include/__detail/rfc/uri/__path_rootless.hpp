#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    // path-rootless = segment-nz *( "/" segment )
    struct path_rootless
    {
    };
    constexpr ReturnType<path_rootless> make_path_rootless(span_param_in sp) noexcept
    {
        if (parse::path_rootless(sp))
            return {};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri
