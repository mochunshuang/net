#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // GMT = %x47.4D.54 ; GMT
    using GMT = StringSensitive<"GMT">;
}; // namespace mcs::abnf::http