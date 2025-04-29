#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>

namespace mcs::abnf::detail
{
    /**
     * @brief OCTET          =  %x00-FF
     *                          ; 8 bits of data
     */
    using octet = std::uint8_t;
    static_assert(std::numeric_limits<octet>::min() == 0 && // NOLINTNEXTLINE
                      std::numeric_limits<octet>::max() == 0xFF,
                  "OCTET range error!");

    using octets_view = std::span<const octet>;
    using octets_view_in = const octets_view &;
    using octet_in = const octet &;

    using consumed_result = std::optional<std::size_t>;
    constexpr auto make_consumed_result(size_t v) noexcept -> consumed_result
    {
        return {v};
    }

    struct parser_ctx
    {
        octets_view root_span;   // NOLINT
        std::size_t cur_index{}; // NOLINT
        std::size_t end_index{}; // NOLINT

        [[nodiscard]] constexpr auto size() const noexcept
        {
            return end_index - cur_index;
        }
        [[nodiscard]] constexpr auto empty() const noexcept
        {
            return end_index == cur_index;
        }
        [[nodiscard]] constexpr auto valid() const noexcept
        {
            return cur_index < end_index;
        }
    };

    constexpr parser_ctx make_parser_ctx(octets_view_in span) noexcept
    {
        return {.root_span = span, .cur_index = 0, .end_index = span.size()};
    }
    using parser_ctx_ref = parser_ctx &;

    inline constexpr auto k_max_octet_value = std::numeric_limits<octet>::max();
    inline constexpr auto k_min_octet_value = std::numeric_limits<octet>::min();
    inline constexpr auto k_max_size_value = std::numeric_limits<std::size_t>::max();
    inline constexpr auto k_min_size_value = std::numeric_limits<std::size_t>::min();

    struct rule_t
    {
    };

}; // namespace mcs::abnf::detail
