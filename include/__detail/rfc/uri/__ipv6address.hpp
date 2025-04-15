#pragma once

#include "../__abnf.hpp"
#include "../__core_types.hpp"

namespace mcs::rfc::uri
{
    struct IPv6address
    {
        IPv6address(id_type id, span_param_in /*sp*/) {}
    };
    constexpr ReturnType<IPv6address> make_IPv6address(span_param_in sp) noexcept
    {
        // TODO(mcs): 多种高级语义化的过程时，给个id是自然的
        if (parse::IPvFuture(sp))
            return IPv6address{0, sp};
        return err_index(0);
    }

}; // namespace mcs::rfc::uri