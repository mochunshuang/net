#pragma once

#include "./__operate_result.hpp"

namespace mcs::abnf::operators
{
    template <typename... Rules>
    inline constexpr auto alternative = [](detail::octets_view_in sp) noexcept { // NOLINT

    };
    template <>
    inline constexpr auto alternative<> =
        [](detail::octets_view_in sp) noexcept { // NOLINT

        };
    template <typename R0>
    inline constexpr auto alternative<R0> =
        [](detail::octets_view_in sp) noexcept { // NOLINT

        };

    template <typename R0, typename R1>
    inline constexpr auto alternative<R0, R1> =
        [](detail::octets_view_in sp) noexcept { // NOLINT

        };
}; // namespace mcs::abnf::operators