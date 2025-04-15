#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct fragment
    {
        explicit fragment(span_param_in /*sp*/) {}
    };
    constexpr ReturnType<fragment> make_fragment(span_param_in sp) noexcept
    {
        if (parse::fragment(sp))
            return fragment{sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri