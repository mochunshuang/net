#pragma once

#include "./__day.hpp"
#include "./__month.hpp"

namespace mcs::abnf::http
{
    // date2 = day "-" month "-" 2DIGIT
    using date2 = sequence<day, Char<'-'>, month, Char<'-'>, times<2, DIGIT>>;
}; // namespace mcs::abnf::http