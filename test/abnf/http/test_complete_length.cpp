#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // complete-length = 1*DIGIT
    static_assert(complete_length("0"_span));
    static_assert(complete_length("012"_span));
    static_assert(complete_length("01234567899999"_span));

    static_assert(not complete_length(""_span));
    static_assert(not complete_length("a"_span));
    static_assert(not complete_length("0."_span));
    static_assert(not complete_length("0.0"_span));

    return 0;
}
// NOLINTEND