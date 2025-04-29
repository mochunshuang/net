#pragma once

#include "./__repetition.hpp"

namespace mcs::abnf::operators
{
    template <detail::rule R>
    using zero_or_more = repetition<0, detail::k_max_size_value, R>;
}; // namespace mcs::abnf::operators