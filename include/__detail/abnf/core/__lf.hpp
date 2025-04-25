#pragma once

#include "../generate/__char.hpp"

namespace mcs::abnf::core
{
    // LF             =  %x0A
    using LF = generate::SensitiveChar<0x0A>;      // NOLINT
    inline constexpr LF lf{};                      // NOLINT
    inline constexpr detail::octet lf_value{0x0A}; // NOLINT
}; // namespace mcs::abnf::core