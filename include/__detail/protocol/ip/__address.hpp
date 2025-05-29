#pragma once

#include "./__address_v4.hpp"
#include "./__address_v6.hpp"
#include "./__bad_address_cast.hpp"
#include <variant>

namespace mcs::protocol::ip
{
    struct address
    {
        static constexpr auto index_v4 = 0; // NOLINT
        static constexpr auto index_v6 = 1; // NOLINT

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
        constexpr explicit address(const address_v4 &a) noexcept : address_{a} {}
        // Postconditions: is_v4() == false and is_v6() == true.
        constexpr explicit address(const address_v6 &a) noexcept : address_{a} {}
        // 21.4.2, assignment:

        // Postconditions: is_v4() == true and is_v6() == false and to_v4() == a.
        address &operator=(const address_v4 &a) noexcept
        {
            address_.emplace<address_v4>(a);
            return *this;
        }
        address &operator=(const address_v6 &a) noexcept
        {
            address_.emplace<address_v6>(a);
            return *this;
        }
        // 21.4.3, members:
        [[nodiscard]] constexpr bool is_v4() const noexcept // NOLINT
        {
            return address_.index() == index_v4;
        }
        [[nodiscard]] constexpr bool is_v6() const noexcept // NOLINT
        {
            return address_.index() == index_v6;
        }
        // Remarks: bad_address_cast if is_v4() == false.
        [[nodiscard]] constexpr address_v4 to_v4() const // NOLINT
        {
            if (not is_v4())
                throw bad_address_cast();
            return std::get<index_v4>(address_);
        }
        [[nodiscard]] constexpr address_v6 to_v6() const // NOLINT
        {
            if (not is_v6())
                throw bad_address_cast();
            return std::get<index_v6>(address_);
        }
        //  If is_v4(), returns std::get<0>(address_).is_unspecified(). Otherwise returns
        //  std::get<1>(address_).is_unspecified().
        [[nodiscard]] constexpr bool is_unspecified() const noexcept // NOLINT
        {
            if (is_v4())
                return std::get<index_v4>(address_).is_unspecified();
            return std::get<index_v6>(address_).is_unspecified();
        }
        [[nodiscard]] constexpr bool is_loopback() const noexcept // NOLINT
        {
            if (is_v4())
                return std::get<index_v4>(address_).is_loopback();
            return std::get<index_v6>(address_).is_loopback();
        }
        [[nodiscard]] constexpr bool is_multicast() const noexcept // NOLINT
        {
            if (is_v4())
                return std::get<index_v4>(address_).is_multicast();
            return std::get<index_v6>(address_).is_multicast();
        }

        // comparisons
        friend constexpr bool operator==(const address &, const address &) = default;
        friend constexpr auto operator<=>(const address &a,
                                          const address &b) noexcept = default;

      private:
        std::variant<address_v4, address_v6> address_;
    };
}; // namespace mcs::protocol::ip