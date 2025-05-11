#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto day_name = make_pass_test<mcs::abnf::http::day_name>();
    constexpr auto not_day_name = make_unpass_test<mcs::abnf::http::day_name>();
    static_assert(day_name("Mon"_span) && day_name("Tue"_span) && day_name("Wed"_span) &&
                  day_name("Thu"_span) && day_name("Fri"_span) && day_name("Sat"_span) &&
                  day_name("Sun"_span));

    static_assert(not not_day_name("mon"_span));
    return 0;
}
// NOLINTEND