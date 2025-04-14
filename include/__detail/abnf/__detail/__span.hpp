#pragma once

namespace mcs::abnf::__detail
{
    struct span
    {
        int start{};
        int end{};
        bool operator==(const span &) const = default; // OK
    };
    constexpr inline span empty_spany{}; // NOLINT

}; // namespace mcs::abnf::__detail