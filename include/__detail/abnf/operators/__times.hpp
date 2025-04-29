#pragma once

#include "./__repetition.hpp"

namespace mcs::abnf::operators
{
    template <size_t N, detail::rule R>
    using times = repetition<N, N, R>;
}; // namespace mcs::abnf::operators