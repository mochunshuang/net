#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    // privateuse    = "x" 1*("-" (1*8alphanum))
    constexpr auto privateuse_pass = make_pass_test<privateuse>();
    constexpr auto privateuse_fail = make_unpass_test<privateuse>();
    static_assert(privateuse_pass("x-foo"_span));      // 基础格式
    static_assert(privateuse_pass("x-123-a1b2"_span)); // 多段扩展
    static_assert(privateuse_pass("X-foo"_span));      // 大写x

    static_assert(!privateuse_fail("x--empty"_span)); // 空子标签

    return 0;
}
// NOLINTEND