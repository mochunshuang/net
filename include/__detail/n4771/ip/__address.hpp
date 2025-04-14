#pragma once
#include "./__typefwd.hpp"
#include "./__address_v4.hpp"
#include "./__address_v6.hpp"
#include "__bad_address_cast.hpp"

namespace mcs::net::ip
{
    class address
    {
      public:
        // 21.4.1, constructors:
        // Postconditions: is_v4() == true, is_v6() == false, and is_unspecified() ==
        // true.
        constexpr address() noexcept = default;
        address(address &&) noexcept = default;
        address &operator=(address &&) noexcept = default;
        constexpr address(const address &a) noexcept = default;
        address &operator=(const address &a) noexcept = default;
        ~address() noexcept = default;

        //  Postconditions: is_v4() == true and is_v6() == false
        constexpr explicit address(const address_v4 &a) noexcept : v4_(a) {}
        // Postconditions: is_v4() == false and is_v6() == true.
        constexpr explicit address(const address_v6 &a) noexcept
            : type_(protocol_type::IPV6), v6_(a)
        {
        }
        // 21.4.2, assignment:

        // Postconditions: is_v4() == true and is_v6() == false and to_v4() == a.
        address &operator=(const address_v4 &a) noexcept
        {
            type_ = protocol_type::IPV4;
            v4_ = a;
            v6_ = {};
            return *this;
        }
        address &operator=(const address_v6 &a) noexcept
        {
            type_ = protocol_type::IPV6;
            v4_ = {};
            v6_ = a;
            return *this;
        }
        // 21.4.3, members:
        [[nodiscard]] constexpr bool is_v4() const noexcept // NOLINT
        {
            return type_ == protocol_type::IPV4;
        }
        [[nodiscard]] constexpr bool is_v6() const noexcept // NOLINT
        {
            return type_ == protocol_type::IPV6;
        }
        // Remarks: bad_address_cast if is_v4() == false.
        [[nodiscard]] constexpr address_v4 to_v4() const // NOLINT
        {
            if (not is_v4())
                throw bad_address_cast();
            return v4_;
        }
        [[nodiscard]] constexpr address_v6 to_v6() const // NOLINT
        {
            if (not is_v6())
                throw bad_address_cast();
            return v6_;
        }
        //  If is_v4(), returns v4_.is_unspecified(). Otherwise returns
        //  v6_.is_unspecified().
        [[nodiscard]] constexpr bool is_unspecified() const noexcept // NOLINT
        {
            if (is_v4())
                return v4_.is_unspecified();
            return v6_.is_unspecified();
        }
        [[nodiscard]] constexpr bool is_loopback() const noexcept // NOLINT
        {
            if (is_v4())
                return v4_.is_loopback();
            return v6_.is_loopback();
        }
        [[nodiscard]] constexpr bool is_multicast() const noexcept // NOLINT
        {
            if (is_v4())
                return v4_.is_multicast();
            return v6_.is_multicast();
        }
        // TODO(mcs):
        template <class Allocator = std::allocator<char>>
        std::basic_string<char, std::char_traits<char>, Allocator> to_string(
            const Allocator &a = Allocator()) const;

        // comparisons
        friend constexpr bool operator==(const address &, const address &) = default;
        friend constexpr auto operator<=>(const address &a,
                                          const address &b) noexcept = default;

      private:
        protocol_type::protocol type_{protocol_type::IPV4};
        address_v4 v4_; // exposition only
        address_v6 v6_; // exposition only
    };
}; // namespace mcs::net::ip