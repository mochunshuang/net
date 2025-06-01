#pragma once

#include <limits>
#include <optional>
#include <span>

namespace mcs::abnf::tool
{
    template <typename T>
    static constexpr auto hex_span_to_size_value(const std::span<T> &data) noexcept
        -> std::optional<size_t>
    {
        using CharType = std::remove_cv_t<T>;
        static_assert(sizeof(CharType) == 1, "Only byte-sized types supported");

        constexpr auto k_hex_char_to_value =
            [](std::uint8_t c) noexcept -> std::optional<std::uint8_t> {
            if (c >= '0' && c <= '9')
                return c - '0';
            if (c >= 'A' && c <= 'F')
                return c - 'A' + 10; // NOLINT
            if (c >= 'a' && c <= 'f')
                return c - 'a' + 10; // NOLINT
            return std::nullopt;
        };

        size_t value = 0;
        bool valid_digit_found = false;
        for (size_t i = 0; i < data.size(); ++i)
        {
            const auto k_digit = k_hex_char_to_value(data[i]);
            if (!k_digit)
            {
                if (!valid_digit_found)
                    return std::nullopt;
                break;
            }
            // check value
            if (value > (std::numeric_limits<size_t>::max() >> 4))
            {
                return std::nullopt;
            }
            value = (value << 4) | *k_digit;
            valid_digit_found = true;
        }
        if (!valid_digit_found)
            return std::nullopt;
        return value;
    }
}; // namespace mcs::abnf::tool