#pragma once

#include "__span.hpp"
#include <cstdint>
#include <limits>
#include <span>

namespace mcs::abnf::__detail
{
    /**
     * @brief OCTET          =  %x00-FF
     *                          ; 8 bits of data
     */
    using OCTET = std::uint8_t;
    static_assert(std::numeric_limits<OCTET>::min() == 0 && // NOLINTNEXTLINE
                      std::numeric_limits<OCTET>::max() == 0xFF,
                  "OCTET range error!");

    using span_param_in = const std::span<const OCTET> &;
    using octet_param_in = const OCTET &;
    using abnf_span_in = const abnf::__detail::span &;
    inline constexpr auto empty_span_param = std::span<const OCTET>{}; // NOLINT

}; // namespace mcs::abnf::__detail