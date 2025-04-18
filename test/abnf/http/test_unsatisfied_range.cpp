#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // unsatisfied-range = "*/" complete-length
    static_assert(not unsatisfied_range("a"_span));
    static_assert(not unsatisfied_range("6a"_span));

    static_assert(not unsatisfied_range("*/"_span));

    static_assert(unsatisfied_range("*/0"_span));
    static_assert(unsatisfied_range("*/01"_span));
    static_assert(unsatisfied_range("*/010"_span));
    static_assert(unsatisfied_range("*/010"_span));
    static_assert(unsatisfied_range("*/999999"_span));

    return 0;
}
// NOLINTEND