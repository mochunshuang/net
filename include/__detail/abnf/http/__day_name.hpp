#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    /*
    day-name = %x4D.6F.6E ; Mon
            / %x54.75.65 ; Tue
            / %x57.65.64 ; Wed
            / %x54.68.75 ; Thu
            / %x46.72.69 ; Fri
            / %x53.61.74 ; Sat
            / %x53.75.6E ; Sun
    */
    using day_name = alternative<StringSensitive<"Mon">, StringSensitive<"Tue">,
                                 StringSensitive<"Wed">, StringSensitive<"Thu">,
                                 StringSensitive<"Fri">, StringSensitive<"Sat">,
                                 StringSensitive<"Sun">>;

}; // namespace mcs::abnf::http