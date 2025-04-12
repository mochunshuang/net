#pragma once

#include "./__uri.hpp"
#include "./__relative_ref.hpp"

namespace mcs::ABNF::URI
{
    // URI-reference = URI / relative-ref
    constexpr bool uri_reference(default_span_t sp) noexcept
    {
        return uri(sp) || relative_ref(sp);
    }
}; // namespace mcs::ABNF::URI