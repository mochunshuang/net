#pragma once
#include <array>

#include <string>
#include <system_error>

#include "../__core_types.hpp"
#include "./__address_v4.hpp"
#include "./__bad_address_cast.hpp"

namespace mcs::net::ip
{
    class address_v6 // NOLINT
    {
      public:
        // 21.6.1, types:
        using bytes_type = std::array<unsigned char, 16>; // NOLINT

        // 21.6.2, constructors:
        // Postconditions: is_unspecified() == true and scope_id() == 0.
        constexpr address_v6() noexcept = default;
        constexpr address_v6(const address_v6 &a) noexcept = default;
        //  Postconditions: to_bytes() == bytes and scope_id() == scope.
        constexpr explicit address_v6(const bytes_type &bytes,
                                      scope_id_type scope = 0) noexcept
            : address_{.s6_addr = bytes}, scope_{scope}
        {
        }

        // 21.6.3, members:
        // Postconditions: scope_id() == id.
        void scope_id(scope_id_type id) noexcept // NOLINT
        {
            scope_ = id;
        }
        [[nodiscard]] constexpr scope_id_type scope_id() const noexcept // NOLINT
        {
            return scope_;
        }
        // Returns: *this == make_address_v6("::")
        [[nodiscard]] constexpr bool is_unspecified() const noexcept; // NOLINT
        [[nodiscard]] constexpr bool is_loopback() const noexcept;    // NOLINT

        [[nodiscard]] constexpr bool is_multicast() const noexcept // NOLINT
        {
            bytes_type b = to_bytes();
            return b[0] == 0xFF; // NOLINT
        }
        // 前缀规则：链路本地地址的前 10 位固定为 1111111010（即 FE80::/10）
        // 第 2 字节（b[1]）：前 2 位必须是 10（二进制 10xxxxxx），即 0x80~0xBF
        // NOTE: 0XFE == 11111110 and 0XC0 == 11000000 and 0x80 == 10000000
        [[nodiscard]] constexpr bool is_link_local() const noexcept // NOLINT
        {
            bytes_type b = to_bytes();
            return b[0] == 0xFE && (b[1] & 0xC0) == 0x80; // NOLINT
        }
        // 站点本地地址的前 10 位固定为 1111111011（即 FEC0::/10）
        // 前 2 位必须是 11（二进制 11xxxxxx），即 0xC0~0xFF
        [[nodiscard]] constexpr bool is_site_local() const noexcept // NOLINT
        {
            bytes_type b = to_bytes();
            return b[0] == 0xFE && (b[1] & 0xC0) == 0xC0; // NOLINT
        }
        [[nodiscard]] constexpr bool is_v4_mapped() const noexcept // NOLINT
        {
            bytes_type b = to_bytes(); // NOLINTBEGIN
            return b[0] == 0 && b[1] == 0 && b[2] == 0 && b[3] == 0 && b[4] == 0 &&
                   b[5] == 0 && b[6] == 0 && b[7] == 0 && b[8] == 0 && b[9] == 0 &&
                   b[10] == 0xFF && b[11] == 0xFF; // NOLINTEND
        }
        // Returns: is_multicast() && (to_bytes()[1] & 0x0F) == 0x01.
        [[nodiscard]] constexpr bool is_multicast_node_local() const noexcept // NOLINT
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x01; // NOLINT
        }
        // Returns: is_multicast() && (to_bytes()[1] & 0x0F) == 0x02
        [[nodiscard]] constexpr bool is_multicast_link_local() const noexcept // NOLINT
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x02; // NOLINT
        }
        // Returns: is_multicast() && (to_bytes()[1] & 0x0F) == 0x05
        [[nodiscard]] constexpr bool is_multicast_site_local() const noexcept // NOLINT
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x05; // NOLINT
        }
        [[nodiscard]] constexpr bool is_multicast_org_local() const noexcept // NOLINT
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x08; // NOLINT
        }
        [[nodiscard]] constexpr bool is_multicast_global() const noexcept // NOLINT
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x0E; // NOLINT
        }
        constexpr bytes_type to_bytes() const noexcept // NOLINT
        {
            return address_.s6_addr;
        }
        // TODO(mcs):
        template <class Allocator = std::allocator<char>>
        std::basic_string<char, std::char_traits<char>, Allocator> to_string(
            const Allocator &a = Allocator()) const;
        // 21.6.4, static members:
        // An address a such that the a.is_unspecified() == true and a.scope_id() == 0.
        static constexpr address_v6 any() noexcept
        {
            return {};
        }
        //  Returns: An address a such that the a.is_loopback() == true and a.scope_id()
        //  == 0.
        static constexpr address_v6 loopback() noexcept;

        // 21.6.5, address_v4 comparisons:
        friend constexpr bool operator==(const address_v6 &,
                                         const address_v6 &) = default;
        friend constexpr auto operator<=>(const address_v6 &a,
                                          const address_v6 &b) noexcept = default;

      private:
        // The underlying IPv6 address.
        in6_addr_type address_{};

        // The scope ID associated with the address.
        scope_id_type scope_{};
    };

    // TODO(mcs):
    // 21.6.6, address_v6 creation:
    constexpr address_v6 make_address_v6(const address_v6::bytes_type &bytes,
                                         scope_id_type scope_id = 0);
    constexpr address_v6 make_address_v6(v4_mapped_t, const address_v4 &a) noexcept;
    address_v6 make_address_v6(const char *str);
    address_v6 make_address_v6(const char *str, std::error_code &ec) noexcept;
    address_v6 make_address_v6(const std::string &str);
    address_v6 make_address_v6(const std::string &str, std::error_code &ec) noexcept;
    address_v6 make_address_v6(std::string_view str);
    address_v6 make_address_v6(std::string_view str, std::error_code &ec) noexcept;

    constexpr bool address_v6::is_unspecified() const noexcept
    {
        return *this == make_address_v6("::");
    }

    constexpr bool address_v6::is_loopback() const noexcept
    {
        return *this == make_address_v6("::1");
    }

    constexpr address_v4 make_address_v4(v4_mapped_t /*unused*/, const address_v6 &a)
    {
        if (not a.is_v4_mapped())
            throw bad_address_cast();
        address_v6::bytes_type v6b = a.to_bytes(); // NOLINTNEXTLINE
        address_v4::bytes_type v4b({v6b[12], v6b[13], v6b[14], v6b[15]});
        return address_v4{v4b};
    }

}; // namespace mcs::net::ip