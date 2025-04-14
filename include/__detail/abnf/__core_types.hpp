#pragma once

#include <cstdint>
#include <limits>
#include <span>
#include "./__detail/__result_builder.hpp"
#include "./__detail/__result.hpp"
#include "__detail/__error.hpp"
#include "__detail/__span.hpp"

namespace mcs::abnf
{
    using __detail::result_builder;
    using __detail::result;
    using __detail::error;
    using __detail::span;
    using __detail::invalid_span;
    using __detail::empty_validspan;

    /**
     * @brief OCTET          =  %x00-FF
     *                          ; 8 bits of data
     */
    using OCTET = std::uint8_t;
    static_assert(std::numeric_limits<OCTET>::min() == 0 && // NOLINTNEXTLINE
                      std::numeric_limits<OCTET>::max() == 0xFF,
                  "OCTET range error!");

    using span_param_in = const std::span<const OCTET> &;
    using octet_t = const OCTET &;
    inline constexpr auto empty_span_param = std::span<const OCTET>{}; // NOLINT

}; // namespace mcs::abnf