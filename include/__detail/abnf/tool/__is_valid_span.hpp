

#include "../__detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr auto is_valid_span(__detail::abnf_span_in span) noexcept
    {
        return span != __detail::invalid_span;
    }
}; // namespace mcs::abnf::tool
