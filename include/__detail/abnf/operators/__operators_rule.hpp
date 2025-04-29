#pragma once

namespace mcs::abnf::operators
{
    template <typename T>
    inline constexpr bool is_operators_rule = false; // NOLINT

    template <typename T>
    concept operators_rule = is_operators_rule<T>;

}; // namespace mcs::abnf::operators