#pragma once

#include <array>
#include <compare>
#include <cstdint>
#include <format>
#include <optional>
#include <ostream>
#include <string>
#include <system_error>

#include "../__socket_types.hpp"

namespace mcs::net::ip
{
    class address_v4 // NOLINT
    {
      public:
        using uint_type = std::uint_least32_t;
        // struct bytes_type;
        using bytes_type = std::array<unsigned char, 4>;

        // Postconditions: to_bytes() yields {0, 0, 0, 0} and to_uint() == 0.
        constexpr address_v4() noexcept = default;

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

        // 21.5.3, members: // NOLINTBEGIN
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
            return {static_cast<unsigned char>((address_.s_addr >> 24) & 0xFF),
                    static_cast<unsigned char>((address_.s_addr >> 16) & 0xFF),
                    static_cast<unsigned char>((address_.s_addr >> 8) & 0xFF),
                    static_cast<unsigned char>(address_.s_addr & 0xFF)};
        }
        // Returns: A representation of the address in host byte order (3.1)
        [[nodiscard]] constexpr uint_type to_uint() const noexcept
        {
            return address_.s_addr;
        }
        /**
         * @brief Returns: If successful, the textual representation of the address,
         * determined as if by POSIX inet_ntop when invoked with address family AF_INET.
         * Otherwise basic_string<char, char_traits<char>, Allocator>(a).
         */
        template <class Allocator = std::allocator<char>>
        constexpr std::basic_string<char, std::char_traits<char>, Allocator> to_string(
            const Allocator &a = Allocator()) const // NOLINTEND
        {
            const auto k_bytes = to_bytes();
            std::basic_string<char, std::char_traits<char>, Allocator> s(a);
            std::format_to(std::back_inserter(s), "{}.{}.{}.{}", k_bytes[0], k_bytes[1],
                           k_bytes[2], k_bytes[3]);
            return s;
        }
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
        // uint_type address_{};
        in4_addr_type address_{};
    };

    // 21.5.6, address_v4 creation: // NOLINTBEGIN
    // Returns: address_v4(bytes)
    constexpr address_v4 make_address_v4(const address_v4::bytes_type &bytes) noexcept
    {
        return address_v4(bytes);
    }
    // Returns: address_v4(val).
    constexpr address_v4 make_address_v4(address_v4::uint_type val) noexcept
    {
        return address_v4(val);
    }
    // NOTE: see __address_v6.hpp
    // constexpr address_v4 make_address_v4(v4_mapped_t /*unused*/, const address_v6 &a);

    namespace __detail
    {
        constexpr bool validate_component(std::string_view component,
                                          bool allow_leading_zero) noexcept
        {
            if (component.empty() || component.size() > 3)
                return false;
            if (component.find_first_not_of("0123456789") != std::string_view::npos)
                return false;
            if (!allow_leading_zero && component.size() > 1 && component[0] == '0')
                return false;

            unsigned value = 0;
            for (char c : component)
            {
                value = value * 10 + (c - '0');
                if (value > 0xFF)
                    return false;
            }
            return true;
        } // namespace mcs::net::ip

        constexpr std::array<std::string_view, 4> split_ipv4_components(
            std::string_view str) noexcept
        {
            std::array<std::string_view, 4> components{};
            size_t start = 0, count = 0;
            bool valid = true;

            for (size_t pos = 0; pos <= str.size(); ++pos)
            {
                if (pos == str.size() || str[pos] == '.')
                {
                    if (count >= 4 || pos == start) //  pos == start => "xx..xx"
                    {
                        valid = false;
                        break;
                    }
                    components[count++] = str.substr(start, pos - start);
                    start = pos + 1;
                }
            }
            return (valid && count == 4) ? components : std::array<std::string_view, 4>{};
        }

        constexpr std::optional<address_v4> parse_v4(std::string_view str,
                                                     bool strict) noexcept
        {
            const auto components = split_ipv4_components(str);
            if (components.empty())
                return std::nullopt;

            address_v4::bytes_type bytes{};
            for (size_t i = 0; i < 4; ++i)
            {
                // 修正参数：严格模式禁用前导零
                if (!validate_component(components[i], !strict))
                    return std::nullopt;

                unsigned value = 0;
                for (char c : components[i])
                    value = value * 10 + (c - '0');

                bytes[i] = static_cast<unsigned char>(value);
            }
            return address_v4(bytes);
        }

        constexpr address_v4 make_address_v4(std::string_view str)
        {
            if (auto parsed = parse_v4(str, true))
                return *parsed;
            throw std::invalid_argument("Invalid IPv4 address");
        }

        constexpr address_v4 make_address_v4(std::string_view str,
                                             std::error_code &ec) noexcept
        {
            if (auto parsed = parse_v4(str, true))
            {
                ec.clear();
                return *parsed;
            }
            ec = std::make_error_code(std::errc::invalid_argument);
            return address_v4{};
        }
    }; // namespace __detail
    // constexpr address_v4 make_address_v4(v4_mapped_t, const address_v6 &a);
    /**
     * @brief
     * Effects: Converts a textual representation of an address into a
     * corresponding address_v4 value, as if by POSIX inet_pton when invoked with
     * address family AF_INET Returns: If successful, an address_v4 value
     * corresponding to the string str. Otherwise address_v4(). Error conditions:
     *  errc::invalid_argument — if str is not a valid textual representation of an
     * IPv4 address.
     */
    // Compile-time parsing for string literals

    constexpr inline address_v4 make_address_v4(const char *str)
    {
        return __detail::make_address_v4(std::string_view(str));
    }

    constexpr inline address_v4 make_address_v4(const char *str,
                                                std::error_code &ec) noexcept
    {
        return __detail::make_address_v4(std::string_view(str), ec);
    }
    constexpr inline address_v4 make_address_v4(const std::string &str)
    {
        return __detail::make_address_v4(std::string_view(str));
    }
    constexpr inline address_v4 make_address_v4(const std::string &str,
                                                std::error_code &ec) noexcept
    {
        return __detail::make_address_v4(std::string_view(str), ec);
    }
    constexpr inline address_v4 make_address_v4(std::string_view str)
    {
        return __detail::make_address_v4(str);
    }
    constexpr inline address_v4 make_address_v4(std::string_view str,
                                                std::error_code &ec) noexcept // NOLINTEND
    {
        return __detail::make_address_v4(str, ec);
    }

    // 21.5.7, address_v4 I/O:
    template <class CharT, class Traits>
    constexpr std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const address_v4 &addr)
    {
        return os << addr.to_string();
    }
}; // namespace mcs::net::ip