#pragma once

#include <cstdint>
namespace mcs::net
{
    using port_type = std::uint_least16_t;
    using scope_id_type = std::uint_least32_t;

    struct v4_mapped_t
    {
    };
    inline constexpr v4_mapped_t v4_mapped; // NOLINT

    struct protocol_type
    {
        enum protocol : uint8_t
        {
            IPV4,
            IPV6
        };

        static consteval protocol v6() noexcept
        {
            return IPV6;
        }
        static consteval protocol v4() noexcept
        {
            return IPV4;
        }

        constexpr bool operator==(protocol other) const noexcept
        {
            return value_ == other;
        }

        // 构造函数（可隐式转换）// NOLINTNEXTLINE
        constexpr protocol_type(protocol p = IPV4) noexcept : value_(p) {}

      private:
        protocol value_;
    };

}; // namespace mcs::net