#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct absolute_URI
    {
        explicit absolute_URI(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<absolute_URI> make_absolute_URI(span_param_in sp) noexcept
    {
        if (parse::absolute_URI(sp))
            return absolute_URI{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri