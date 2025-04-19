#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    static_assert(day_name_l("Monday"_span));
    static_assert(day_name_l("Tuesday"_span) && day_name_l("Tuesday"_span) &&
                  day_name_l("Thursday"_span) && day_name_l("Friday"_span) &&
                  day_name_l("Saturday"_span) && day_name_l("Sunday"_span));
    return 0;
}
// NOLINTEND