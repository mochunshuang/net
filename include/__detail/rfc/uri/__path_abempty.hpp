#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct path_abempty
    {
    };

    constexpr ReturnType<path_abempty> make_path_abempty(span_param_in sp) noexcept
    {
        if (parse::path_abempty(sp))
            return path_abempty{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri
