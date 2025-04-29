#pragma once

#include "../generate/__char.hpp"

namespace mcs::abnf::core
{
    // CR             =  %x0D
    using CR = generate::CharSensitive<0x0D>;      // NOLINT
    inline constexpr detail::octet cr_value{0x0D}; // NOLINT
}; // namespace mcs::abnf::core