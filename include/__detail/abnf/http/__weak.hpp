#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // NOTE: %x57 =='w',%x2F =='/'
    // weak = %x57.2F ; W/
    using weak = sequence<Char<'W'>, Char<'/'>>;
}; // namespace mcs::abnf::http