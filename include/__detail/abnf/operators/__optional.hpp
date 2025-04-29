#pragma once

#include "./__repetition.hpp"

namespace mcs::abnf::operators
{
    template <detail::rule R>
    using optional = repetition<0, 1, R>;
}; // namespace mcs::abnf::operators