#pragma once

#include "./__detail/__error.hpp"
#include "./__detail/__result.hpp"
#include <expected>
#include <limits>
#include <span>

namespace mcs::abnf
{
    using Success = __detail::result;
    using Info = __detail::error;
    using CheckResult = std::expected<Success, Info>;

    constexpr auto Fail(const size_t &idx) noexcept
    {
        return std::unexpected<Info>(idx);
    }

    /**
     * @brief OCTET          =  %x00-FF
     *                          ; 8 bits of data
     */
    using OCTET = std::uint8_t;
    static_assert(std::numeric_limits<OCTET>::min() == 0 && // NOLINTNEXTLINE
                      std::numeric_limits<OCTET>::max() == 0xFF,
                  "OCTET range error!");

    using default_span_t = const std::span<const OCTET> &;
    using octet_t = const OCTET &;
    inline constexpr auto empty_span = std::span<const OCTET>{}; // NOLINT

}; // namespace mcs::abnf