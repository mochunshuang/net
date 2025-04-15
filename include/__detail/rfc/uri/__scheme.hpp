#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct scheme
    {
    };

    constexpr ReturnType<scheme> make_scheme(span_param_in sp) noexcept
    {
        if (parse::scheme(sp))
            return scheme{};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri
