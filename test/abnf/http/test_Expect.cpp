#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Expect = [ expectation *( OWS "," OWS expectation ) ]
    constexpr auto expect_pass = make_pass_test<mcs::abnf::http::Expect>();
    constexpr auto expect_fail = make_unpass_test<mcs::abnf::http::Expect>();
    static_assert(expect_pass("100-continue"_span));
    static_assert(expect_pass("custom=param, another"_span)); // 多期望

    static_assert(expect_fail("invalid=@value"_span)); // 非法参数
    return 0;
}
// NOLINTEND