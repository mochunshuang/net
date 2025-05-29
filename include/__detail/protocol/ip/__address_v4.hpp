#pragma once

#include <cstddef>
#include <cstdint>
#include <array>

#include "./__detail/__type.hpp"

namespace mcs::protocol::ip
{
    struct address_v4
    {
      public:
        /// The type used to represent an address as an unsigned integer.
        using uint_type = std::uint_least32_t;
        /// The type used to represent an address as an array of bytes.
        using bytes_type = std::array<std::byte, 4>;

        // Postconditions: to_bytes() yields {0, 0, 0, 0} and to_uint() == 0.
        constexpr address_v4() noexcept = default;
        ~address_v4() = default;
        address_v4(address_v4 &&) = default;
        address_v4 &operator=(address_v4 &&) = default;

        constexpr address_v4(const address_v4 &a) noexcept = default;
        /**
         * @brief Postconditions:
         * to_bytes() == bytes and to_uint() == (bytes[0] << 24) | (bytes[1] << 16) |
         * (bytes[2] << 8) | bytes[3].
         *
         */
        constexpr address_v4(const bytes_type &bytes) noexcept // NOLINT // NOLINTBEGIN
            : address_((static_cast<uint_type>(bytes[0]) << 24) |
                       (static_cast<uint_type>(bytes[1]) << 16) |
                       (static_cast<uint_type>(bytes[2]) << 8) |
                       static_cast<uint_type>(bytes[3])) // NOLINTEND
        {
        }
        /**
         * @brief Postconditions: to_uint() == val and to_bytes() is:
         * { (val >> 24) & 0xFF, (val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF }
         */
        explicit constexpr address_v4(uint_type val) noexcept : address_(val) {}

        // assignment:
        address_v4 &operator=(const address_v4 &a) noexcept = default;

        // NOLINTBEGIN

        // 21.5.3, members:
        // Returns: to_uint() == 0.
        [[nodiscard]] constexpr bool is_unspecified() const noexcept
        {
            return to_uint() == 0;
        }
        /**
         * @brief 127.0.0.0/8
         * which corresponds to the address range
         * 127.0.0.0 - 127.255.255.255
         * NOTE: 7F == 127
         */
        // Returns: (to_uint() & 0xFF000000) == 0x7F000000.
        [[nodiscard]] constexpr bool is_loopback() const noexcept
        {
            return (to_uint() & 0xFF000000) == 0x7F000000;
        }
        /**
         * @brief 224.0.0.0/4
         * which corresponds to the address range
         * 224.0.0.0 - 239.255.255.255
         * NOTE: E0 == 224, EF == 239
         */
        //  Returns: (to_uint() & 0xF0000000) == 0xE0000000
        [[nodiscard]] constexpr bool is_multicast() const noexcept
        {
            return (to_uint() & 0xF0000000) == 0xE0000000;
        }
        // Returns: A representation of the address in network byte order
        [[nodiscard]] constexpr bytes_type to_bytes() const noexcept
        {
            return {static_cast<std::byte>((address_.value >> 24) & 0xFF),
                    static_cast<std::byte>((address_.value >> 16) & 0xFF),
                    static_cast<std::byte>((address_.value >> 8) & 0xFF),
                    static_cast<std::byte>(address_.value & 0xFF)};
        }
        // Returns: A representation of the address in host byte order (3.1)
        [[nodiscard]] constexpr uint_type to_uint() const noexcept
        {
            return address_.value;
        }

        // NOLINTEND

        // 21.5.4, static members:
        // Returns: address_v4()
        static constexpr address_v4 any() noexcept
        {
            return {};
        }
        // Returns: address_v4(0x7F000001): 127.0.0.1
        static constexpr address_v4 loopback() noexcept
        {
            return address_v4(0x7F000001); // NOLINT
        }
        // Returns: address_v4(0xFFFFFFFF): 255.255.255.255
        static constexpr address_v4 broadcast() noexcept
        {
            return address_v4(0xFFFFFFFF); // NOLINT
        }

        // 21.5.5, address_v4 comparisons:
        friend constexpr bool operator==(const address_v4 &,
                                         const address_v4 &) = default;
        friend constexpr auto operator<=>(const address_v4 &a,
                                          const address_v4 &b) noexcept
            -> std::strong_ordering = default;

      private:
        __detail::address_v4_store address_{};
    };

}; // namespace mcs::protocol::ip