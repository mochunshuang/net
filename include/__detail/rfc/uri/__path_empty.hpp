#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"
#include <expected>

namespace mcs::rfc::uri
{
    struct path_empty
    {
    };
    constexpr ReturnType<path_empty> make_path_empty(span_param_in sp) noexcept
    {
        if (parse::path_empty(sp))
            return {};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri