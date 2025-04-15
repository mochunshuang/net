
#include "../__detail/__types.hpp"

namespace mcs::abnf::tool
{
    constexpr auto make_stdspan(__detail::span_param_in spans,
                                __detail::abnf_span_in s) noexcept
    {
        return spans.subspan(s.start, s.count);
    }

}; // namespace mcs::abnf::tool
