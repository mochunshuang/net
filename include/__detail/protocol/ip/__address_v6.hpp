
#pragma once

#include "./__address_v4.hpp"
#include "./__scope_id_type.hpp"
#include "./__v4_mapped.hpp"
#include "./__detail/__type.hpp"

namespace mcs::protocol::ip
{
    struct address_v6
    {
      public:
        // 21.6.1, types:
        using bytes_type = std::array<unsigned char, 16>; // NOLINT

        // 21.6.2, constructors:
        // Postconditions: is_unspecified() == true and scope_id() == 0.
        constexpr address_v6() noexcept = default;
        ~address_v6() = default;
        address_v6(address_v6 &&) = default;
        address_v6 &operator=(address_v6 &&) = default;
        address_v6 &operator=(const address_v6 &) = default;
        constexpr address_v6(const address_v6 &a) noexcept = default;

        //  Postconditions: to_bytes() == bytes and scope_id() == scope.
        constexpr explicit address_v6(const bytes_type &bytes,
                                      scope_id_type scope = 0) noexcept
            : address_{.value = bytes}, scope_{scope}
        {
        }

        // NOLINTBEGIN

        // 21.6.3, members:
        // Postconditions: scope_id() == id.
        void scope_id(scope_id_type id) noexcept
        {
            scope_ = id;
        }
        [[nodiscard]] constexpr scope_id_type scope_id() const noexcept
        {
            return scope_;
        }
        // Returns: *this == make_address_v6("::")
        [[nodiscard]] constexpr bool is_unspecified() const noexcept
        {
            return false; // TODO
        }
        [[nodiscard]] constexpr bool is_loopback() const noexcept
        {
            // TODO
            return false;
        }

        [[nodiscard]] constexpr bool is_multicast() const noexcept
        {
            bytes_type b = to_bytes();
            return b[0] == 0xFF;
        }
        // 前缀规则：链路本地地址的前 10 位固定为 1111111010（即 FE80::/10）
        // 第 2 字节（b[1]）：前 2 位必须是 10（二进制 10xxxxxx），即 0x80~0xBF
        // NOTE: 0XFE == 11111110 and 0XC0 == 11000000 and 0x80 == 10000000
        [[nodiscard]] constexpr bool is_link_local() const noexcept
        {
            bytes_type b = to_bytes();
            return b[0] == 0xFE && (b[1] & 0xC0) == 0x80;
        }
        // 站点本地地址的前 10 位固定为 1111111011（即 FEC0::/10）
        // 前 2 位必须是 11（二进制 11xxxxxx），即 0xC0~0xFF
        [[nodiscard]] constexpr bool is_site_local() const noexcept
        {
            bytes_type b = to_bytes();
            return b[0] == 0xFE && (b[1] & 0xC0) == 0xC0;
        }
        [[nodiscard]] constexpr bool is_v4_mapped() const noexcept
        {
            bytes_type b = to_bytes();
            return b[0] == 0 && b[1] == 0 && b[2] == 0 && b[3] == 0 && b[4] == 0 &&
                   b[5] == 0 && b[6] == 0 && b[7] == 0 && b[8] == 0 && b[9] == 0 &&
                   b[10] == 0xFF && b[11] == 0xFF;
        }
        // Returns: is_multicast() && (to_bytes()[1] & 0x0F) == 0x01.
        [[nodiscard]] constexpr bool is_multicast_node_local() const noexcept
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x01;
        }
        // Returns: is_multicast() && (to_bytes()[1] & 0x0F) == 0x02
        [[nodiscard]] constexpr bool is_multicast_link_local() const noexcept
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x02;
        }
        // Returns: is_multicast() && (to_bytes()[1] & 0x0F) == 0x05
        [[nodiscard]] constexpr bool is_multicast_site_local() const noexcept
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x05;
        }
        [[nodiscard]] constexpr bool is_multicast_org_local() const noexcept
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x08;
        }
        [[nodiscard]] constexpr bool is_multicast_global() const noexcept
        {
            return is_multicast() && (to_bytes()[1] & 0x0F) == 0x0E;
        }
        [[nodiscard]] constexpr bytes_type to_bytes() const noexcept
        {
            return address_.value;
        }

        // NOLINTEND

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
        __detail::address_v6_store address_{};

        // The scope ID associated with the address.
        scope_id_type scope_{};
    };

    // TODO(mcs):
    // 21.6.6, address_v6 creation:
    constexpr address_v6 make_address_v6(const address_v6::bytes_type &bytes,
                                         scope_id_type scope_id = 0);
    constexpr address_v6 make_address_v6(v4_mapped_t, const address_v4 &a) noexcept;

}; // namespace mcs::protocol::ip
