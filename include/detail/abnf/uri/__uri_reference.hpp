#pragma once

#include "./__uri.hpp"
#include "./__relative_ref.hpp"

namespace mcs::abnf::uri
{
    // URI-reference = URI / relative-ref
    constexpr bool URI_reference(default_span_t sp) noexcept
    {
        return URI(sp) || relative_ref(sp);
    }
}; // namespace mcs::abnf::uri