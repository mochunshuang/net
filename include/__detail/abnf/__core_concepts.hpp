#pragma once

#include "./__detail/__abnf_result.hpp"

namespace mcs::abnf
{
    template <class From, class To>
    concept decays_to = std::same_as<std::decay_t<From>, To>;

    using __detail::abnf_result;

}; // namespace mcs::abnf
