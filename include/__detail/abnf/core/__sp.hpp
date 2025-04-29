#pragma once

#include "../generate/__char.hpp"

namespace mcs::abnf::core
{
    // SP             =  %x20
    using SP = generate::CharSensitive<0x20>;      // NOLINT
    inline constexpr detail::octet sp_value{0x20}; // NOLINT
}; // namespace mcs::abnf::core