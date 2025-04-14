#pragma once

#include <expected>

#include "./__error.hpp"
#include "./__abnf_result.hpp"

namespace mcs::abnf::__detail
{
    template <typename V, typename E = error>
    struct result_builder
    {
        using value_type = V;
        using error_type = E;
        using result_type = std::expected<value_type, error_type>;

        template <typename... Args>
            requires requires {
                result_type(std::in_place, std::forward<Args>(std::declval<Args>())...);
            }
        static constexpr auto success(Args &&...args) noexcept -> abnf_result auto
        {
            return result_type(std::in_place, std::forward<Args>(args)...);
        }

        template <typename... Ts>
            requires requires {
                result_type(std::unexpected(std::forward<Ts>(std::declval<Ts>())...));
            }
        static constexpr auto fail(Ts &&...ts) noexcept -> abnf_result auto
        {
            return result_type(std::unexpected(std::forward<Ts>(ts)...));
        }
    };
}; // namespace mcs::abnf::__detail