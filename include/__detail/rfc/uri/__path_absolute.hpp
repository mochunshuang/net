#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct path_absolute
    {
    };

    constexpr ReturnType<path_absolute> make_path_absolute(span_param_in sp) noexcept
    {
        if (parse::path_absolute(sp))
            return path_absolute{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri
