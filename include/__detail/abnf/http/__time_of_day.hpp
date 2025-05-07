#pragma once

#include "./__hour.hpp"
#include "./__minute.hpp"
#include "./__second.hpp"

namespace mcs::abnf::http
{
    // hour = 2DIGIT
    // minute = 2DIGIT
    // second = 2DIGIT
    // time-of-day = hour ":" minute ":" second
    using time_of_day = sequence<hour, Char<':'>, minute, Char<':'>, second>;
}; // namespace mcs::abnf::http
