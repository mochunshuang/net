#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto day_name_l = make_pass_test<mcs::abnf::http::day_name_l>();
    constexpr auto not_day_name_l = make_unpass_test<mcs::abnf::http::day_name_l>();
    static_assert(day_name_l("Monday"_span));
    static_assert(day_name_l("Tuesday"_span) && day_name_l("Tuesday"_span) &&
                  day_name_l("Thursday"_span) && day_name_l("Friday"_span) &&
                  day_name_l("Saturday"_span) && day_name_l("Sunday"_span));

    static_assert(not not_day_name_l("monday"_span));
    static_assert(not not_day_name_l("sunday"_span));
    return 0;
}
// NOLINTEND