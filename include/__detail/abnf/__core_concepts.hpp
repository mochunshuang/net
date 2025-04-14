#pragma once

#include <concepts>

namespace mcs::abnf
{
    template <class From, class To>
    concept decays_to = std::same_as<std::decay_t<From>, To>;

}; // namespace mcs::abnf
