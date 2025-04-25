#pragma once

#include "./__repetition.hpp"
#include "./__operable_rule.hpp"

namespace mcs::abnf::operators
{
    template <operable_rule R>
    using zero_or_more = repetition<0, detail::k_max_size_value, R>;
}; // namespace mcs::abnf::operators