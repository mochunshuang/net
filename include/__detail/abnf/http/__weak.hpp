#pragma once

#include "../__core_types.hpp"

namespace mcs::abnf::http
{
    // NOTE: %x57 =='w',%x2F =='/'
    // weak = %x57.2F ; W/
    constexpr abnf_result auto weak(span_param_in sp) noexcept
    {
        static_assert(0x57 == 'W' && 0x2F == '/'); // NOLINT
        return sp.size() == 2 && sp[0] == 'W' && sp[1] == '/';
    }
}; // namespace mcs::abnf::http