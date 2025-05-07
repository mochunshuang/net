#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    /*
    day-name-l = %x4D.6F.6E.64.61.79 ; Monday
                / %x54.75.65.73.64.61.79 ; Tuesday
                / %x57.65.64.6E.65.73.64.61.79 ; Wednesday
                / %x54.68.75.72.73.64.61.79 ; Thursday
                / %x46.72.69.64.61.79 ; Friday
                / %x53.61.74.75.72.64.61.79 ; Saturday
                / %x53.75.6E.64.61.79 ; Sunday
    */
    using day_name_l =
        alternative<StringSensitive<"Monday">, StringSensitive<"Tuesday">,
                    StringSensitive<"Wednesday">, StringSensitive<"Thursday">,
                    StringSensitive<"Friday">, StringSensitive<"Saturday">,
                    StringSensitive<"Sunday">>;
}; // namespace mcs::abnf::http