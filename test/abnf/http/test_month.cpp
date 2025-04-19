#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    static_assert(month("Jan"_span) && month("Feb"_span) && month("Mar"_span) &&
                  month("Apr"_span) && month("May"_span) && month("Jun"_span) &&
                  month("Jul"_span) && month("Aug"_span) && month("Sep"_span) &&
                  month("Oct"_span) && month("Nov"_span) && month("Dec"_span));

    return 0;
}
// NOLINTEND