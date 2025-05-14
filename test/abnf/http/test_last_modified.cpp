#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // Last-Modified = HTTP-date
    constexpr auto last_modified_pass = make_pass_test<mcs::abnf::http::last_Modified>();
    constexpr auto last_modified_fail =
        make_unpass_test<mcs::abnf::http::last_Modified>();
    static_assert(last_modified_pass("Fri, 15 Sep 2023 08:00:00 GMT"_span));
    static_assert(not last_modified_fail("2023-09-15T08:00:00Z"_span)); // ISO格式

    // An example of its use is
    // Last-Modified: Tue, 15 Nov 1994 12:45:26 GMT
    static_assert(last_modified_pass("Tue, 15 Nov 1994 12:45:26 GMT"_span));
    return 0;
}
// NOLINTEND