#include "../__detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr auto is_empty_span(__detail::abnf_span_in span) noexcept
    {
        return span.count == 0;
    }
}; // namespace mcs::abnf::tool
