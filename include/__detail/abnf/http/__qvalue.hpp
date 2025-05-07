#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
    using qvalue = alternative<
        sequence<CharSensitive<'0'>,
                 optional<sequence<CharSensitive<'.'>, times<3, DIGIT>>>>,
        sequence<CharSensitive<'1'>,
                 optional<sequence<CharSensitive<'.'>, times<3, CharSensitive<'0'>>>>>>;
}; // namespace mcs::abnf::http