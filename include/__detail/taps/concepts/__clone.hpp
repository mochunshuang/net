#pragma once

#include <concepts>

namespace mcs::taps::concepts
{
    template <typename T>
    concept clone = requires(T t) {
        { t.clone() } noexcept -> std::same_as<T>;
    };

}; // namespace mcs::taps::concepts