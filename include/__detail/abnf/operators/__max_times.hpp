#pragma once

#include "./__repetition.hpp"

namespace mcs::abnf::operators
{
    template <size_t N, detail::rule R>
    using max_times = repetition<0, N, R>;
}; // namespace mcs::abnf::operators