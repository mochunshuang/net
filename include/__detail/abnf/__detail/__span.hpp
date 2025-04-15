#pragma once

#include <cstddef>
#include <limits>

namespace mcs::abnf::__detail
{
    struct span
    {
        static constexpr auto INVALID = std::numeric_limits<std::size_t>::max(); // NOLINT
        std::size_t start{INVALID};                                              // NOLINT
        std::size_t count{};                                                     // NOLINT
        bool operator==(const span &) const = default;
    };
    // NOTE: invalid is an initial statement
    constexpr inline span invalid_span{}; // NOLINT

}; // namespace mcs::abnf::__detail