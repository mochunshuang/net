#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct relative_part
    {
        explicit relative_part(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<relative_part> make_relative_part(span_param_in sp) noexcept
    {
        if (parse::relative_part(sp))
            return relative_part{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri