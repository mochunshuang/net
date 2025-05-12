#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // If-None-Match = "*" / [ entity-tag *( OWS "," OWS entity-tag ) ]
    constexpr auto if_none_match_pass = make_pass_test<mcs::abnf::http::If_None_Match>();
    constexpr auto if_none_match_fail =
        make_unpass_test<mcs::abnf::http::If_None_Match>();
    static_assert(if_none_match_pass("\"v1\", \"v2\""_span)); // 多版本标签
    static_assert(if_none_match_fail("W/\"tag\" "_span));     // 尾部空格

    return 0;
}
// NOLINTEND