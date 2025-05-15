#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    // alphanum      = (ALPHA / DIGIT)     ; letters and numbers
    constexpr auto alphanum_pass = make_pass_test<alphanum>();
    constexpr auto alphanum_fail = make_unpass_test<alphanum>();

    static_assert(alphanum_pass("A"_span));  // 大写字母
    static_assert(alphanum_pass("z"_span));  // 小写字母
    static_assert(alphanum_pass("9"_span));  // 数字
    static_assert(!alphanum_fail("!"_span)); // 非法符号

    return 0;
}
// NOLINTEND