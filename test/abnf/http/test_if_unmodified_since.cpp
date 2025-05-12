#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // If-Unmodified-Since = HTTP-date
    constexpr auto if_unmodified_pass =
        make_pass_test<mcs::abnf::http::If_Unmodified_Since>();
    static_assert(if_unmodified_pass("Sun, 31 Dec 2023 23:59:59 GMT"_span));

    return 0;
}
// NOLINTEND