#pragma once

#include "./__repetition.hpp"
#include "./__operable_rule.hpp"

namespace mcs::abnf::operators
{
    template <size_t N, operable_rule R>
    using times = repetition<N, N, R>;
}; // namespace mcs::abnf::operators