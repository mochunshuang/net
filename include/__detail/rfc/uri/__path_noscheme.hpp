#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct path_noscheme
    {
    };
    constexpr ReturnType<path_noscheme> make_path_noscheme(span_param_in sp) noexcept
    {
        if (parse::path_noscheme(sp))
            return {};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri