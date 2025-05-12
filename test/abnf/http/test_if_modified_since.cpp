#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // If-Modified-Since = HTTP-date
    constexpr auto if_modified_pass =
        make_pass_test<mcs::abnf::http::If_Modified_Since>();
    static_assert(if_modified_pass("Wed, 21 Oct 2023 07:28:00 GMT"_span));
    static_assert(if_modified_pass("Mon, 01 Jan 2001 00:00:00 GMT"_span));

    return 0;
}
// NOLINTEND