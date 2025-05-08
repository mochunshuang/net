#pragma once

#include "./__hour.hpp"
#include "./__minute.hpp"
#include "./__second.hpp"

namespace mcs::abnf::http
{
    // time-of-day = hour ":" minute ":" second
    using time_of_day = sequence<hour, Char<':'>, minute, Char<':'>, second>;
}; // namespace mcs::abnf::http
