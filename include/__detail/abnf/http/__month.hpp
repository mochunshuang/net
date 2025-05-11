#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{

    /*
    month = %x4A.61.6E ; Jan
        / %x46.65.62 ; Feb
        / %x4D.61.72 ; Mar
        / %x41.70.72 ; Apr
        / %x4D.61.79 ; May
        / %x4A.75.6E ; Jun
        / %x4A.75.6C ; Jul
        / %x41.75.67 ; Aug
        / %x53.65.70 ; Sep
        / %x4F.63.74 ; Oct
        / %x4E.6F.76 ; Nov
        / %x44.65.63 ; Dec
    */
    using month = alternative<
        StringSensitive<"Jan">, StringSensitive<"Feb">, StringSensitive<"Mar">,
        StringSensitive<"Apr">, StringSensitive<"May">, StringSensitive<"Jun">,
        StringSensitive<"Jul">, StringSensitive<"Aug">, StringSensitive<"Sep">,
        StringSensitive<"Oct">, StringSensitive<"Nov">, StringSensitive<"Dec">>;
} // namespace mcs::abnf::http