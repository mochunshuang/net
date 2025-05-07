#pragma once

#include "../generate/__char.hpp"

namespace mcs::abnf::core
{
    // DQUOTE         =  %x22 ;" (Double Quote)
    using DQUOTE = generate::Char<0x22>;               // NOLINT
    inline constexpr detail::octet dquote_value{0x22}; // NOLINT
}; // namespace mcs::abnf::core