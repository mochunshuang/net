#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct relative_ref
    {
        explicit relative_ref(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<relative_ref> make_relative_ref(span_param_in sp) noexcept
    {
        if (parse::relative_ref(sp))
            return relative_ref{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri