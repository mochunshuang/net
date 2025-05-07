#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // NOTE: %x57 =='w',%x2F =='/'
    // weak = %x57.2F ; W/
    using weak = sequence<CharSensitive<'W'>, CharSensitive<'/'>>;
}; // namespace mcs::abnf::http