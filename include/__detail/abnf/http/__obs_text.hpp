#pragma once

#include "../__core_types.hpp"
#include <limits>
#include <type_traits>

namespace mcs::abnf::http
{
    // obs-text = %x80-FF
    constexpr abnf_result auto obs_text(octet_param_in c) noexcept
    { // NOLINTNEXTLINE
        static_assert(std::numeric_limits<std::decay_t<decltype(c)>>::max() == 0xFF);
        return c >= 0x80; // NOLINT
    }
}; // namespace mcs::abnf::http