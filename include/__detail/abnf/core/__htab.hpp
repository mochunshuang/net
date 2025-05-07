#pragma once

#include "../generate/__char.hpp"

namespace mcs::abnf::core
{
    // HTAB           =  %x09; horizontal tab
    using HTAB = generate::Char<0x09>;               // NOLINT
    inline constexpr detail::octet htab_value{0x09}; // NOLINT
}; // namespace mcs::abnf::core