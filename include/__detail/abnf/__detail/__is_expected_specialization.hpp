#pragma once

#include <expected>

namespace mcs::abnf::__detail
{
    namespace __detail
    {
        template <typename T>
        inline constexpr bool is_expected = false; // NOLINT
        template <typename V, typename E>          // NOLINTNEXTLINE
        inline constexpr bool is_expected<std::expected<V, E>> = true;
    }; // namespace __detail

    template <typename T>
    concept is_expected_specialization = __detail::is_expected<T>;

}; // namespace mcs::abnf::__detail