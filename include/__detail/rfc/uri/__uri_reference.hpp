#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct URI_reference
    {
        explicit URI_reference(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<URI_reference> make_URI_reference(span_param_in sp) noexcept
    {
        if (parse::URI_reference(sp))
            return URI_reference{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri