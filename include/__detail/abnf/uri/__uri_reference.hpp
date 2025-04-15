#pragma once

#include "./__uri.hpp"
#include "./__relative_ref.hpp"

namespace mcs::abnf::uri
{
    // URI-reference = URI / relative-ref
    constexpr bool URI_reference(span_param_in sp) noexcept
    {
        // TODO(mcs):
        return URI(sp) || relative_ref(sp);
    }
}; // namespace mcs::abnf::uri