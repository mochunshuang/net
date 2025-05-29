#pragma once

#include "../__core_types.hpp"
#include "./__is_endpoint.hpp"
#include <concepts>

namespace mcs::taps::concepts
{
    template <typename T>
    concept remote_endpoint =
        is_endpoint<T> &&
        std::derived_from<typename T::endpoint_concept, remote_endpoint_t>;
}; // namespace mcs::taps::concepts