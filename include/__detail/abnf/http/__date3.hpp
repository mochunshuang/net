#pragma once

#include "./__month.hpp"

namespace mcs::abnf::http
{
    // date3 = month SP ( 2DIGIT / ( SP DIGIT ) )
    using date3 = sequence<month, SP, alternative<times<2, DIGIT>, sequence<SP, DIGIT>>>;
}; // namespace mcs::abnf::http
