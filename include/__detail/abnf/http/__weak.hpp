#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::http
{
    // NOTE: %x57 =='w',%x2F =='/'
    // weak = %x57.2F ; W/
    constexpr abnf_result auto weak(octet_param_in a, octet_param_in b) noexcept
    {
        static_assert(0x57 == 'W' && 0x2F == '/'); // NOLINT
        return a == 'W' && b == '/';
    }
}; // namespace mcs::abnf::http