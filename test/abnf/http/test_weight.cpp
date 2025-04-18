#include "../test_head.hpp"

#include <cassert>
#include <string_view>

// NOLINTBEGIN

using namespace mcs::abnf::http;

int main()
{
    // weight = OWS ";" OWS "q=" qvalue

    constexpr auto OWS0 = "";      // NOLINT
    constexpr auto OWS1 = "     "; // NOLINT
    static_assert(OWS(""_span));
    static_assert(OWS("     "_span));

    // test
    static_assert(qvalue("0.111"_span));
    static_assert(not qvalue("0.1111"_span));
    static_assert(not qvalue("0.1110"_span));

    static_assert(not weight("0.111"_span));
    static_assert(weight(";q=0.111"_span));
    static_assert(weight(" ;q=0.111"_span));
    static_assert(weight(" ;         q=0.111"_span));
    static_assert(weight("  ;         q=0.111"_span));

    static_assert(not weight("0.111"_span));
    static_assert(not weight(";q= 0.111"_span));
    static_assert(not weight(" ;q= 0.111"_span));
    static_assert(not weight(" ;         q= 0.111"_span));
    static_assert(not weight("  ;         q= 0.111"_span));

    static_assert(not weight(";q=0.1110"_span));
    static_assert(not weight(";q=0.1110"_span));
    static_assert(not weight(" ;q=0.1110"_span));
    static_assert(not weight(" ;         q=0.1110"_span));
    static_assert(not weight("  ;         q=0.1110"_span));

    return 0;
}
// NOLINTEND