#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // qvalue = ( "0" [ "." *3DIGIT ] ) / ( "1" [ "." *3"0" ] )
    static_assert(not qvalue(""_span));
    static_assert(not qvalue("2"_span));

    static_assert(qvalue("0"_span));
    static_assert(qvalue("0."_span));
    static_assert(qvalue("0.1"_span));
    static_assert(qvalue("0.11"_span));
    static_assert(qvalue("0.111"_span));
    static_assert(qvalue("0.101"_span));
    static_assert(qvalue("0.999"_span));

    static_assert(not qvalue("0.1111"_span));

    static_assert(not qvalue("0.11x"_span));

    static_assert(not qvalue("1.11x"_span));
    static_assert(not qvalue("1.11"_span));

    static_assert(not qvalue("1.1"_span));

    static_assert(qvalue("1."_span));
    static_assert(qvalue("1"_span));
    static_assert(qvalue("1.0"_span));
    static_assert(qvalue("1.00"_span));
    static_assert(qvalue("1.000"_span));

    static_assert(not qvalue("1.0000"_span));

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND