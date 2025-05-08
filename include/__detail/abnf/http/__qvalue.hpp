#pragma once

#include "../__abnf.hpp"

namespace mcs::abnf::http
{
    // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
    using qvalue = alternative<
        sequence<Char<'0'>, optional<sequence<Char<'.'>, repetition<0, 3, DIGIT>>>>,
        sequence<Char<'1'>, optional<sequence<Char<'.'>, repetition<0, 3, Char<'0'>>>>>>;
}; // namespace mcs::abnf::http