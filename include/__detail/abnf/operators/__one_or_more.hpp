#pragma once

#include "./__repetition.hpp"
#include "./__operable_rule.hpp"

namespace mcs::abnf::operators
{
    template <operable_rule R>
    using one_or_more = repetition<1, detail::k_max_size_value, R>;
}; // namespace mcs::abnf::operators