#pragma once

#include "./__repetition.hpp"
#include "./__operable_rule.hpp"

namespace mcs::abnf::operators
{
    template <operable_rule R>
    using optional = repetition<0, 1, R>;
}; // namespace mcs::abnf::operators