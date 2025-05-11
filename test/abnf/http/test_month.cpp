#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    constexpr auto month = make_pass_test<mcs::abnf::http::month>();
    constexpr auto not_month = make_unpass_test<mcs::abnf::http::month>();
    static_assert(month("Jan"_span) && month("Feb"_span) && month("Mar"_span) &&
                  month("Apr"_span) && month("May"_span) && month("Jun"_span) &&
                  month("Jul"_span) && month("Aug"_span) && month("Sep"_span) &&
                  month("Oct"_span) && month("Nov"_span) && month("Dec"_span));

    static_assert(not not_month("jan"_span));
    static_assert(not not_month("dec"_span));

    return 0;
}
// NOLINTEND