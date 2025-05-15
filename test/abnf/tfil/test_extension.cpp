#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
    *singleton     = DIGIT               ; 0 - 9
                   / %x41-57             ; A - W
                   / %x59-5A             ; Y - Z
                   / %x61-77             ; a - w
                   / %x79-7A             ; y - z
    */
    // extension     = singleton 1*("-" (2*8alphanum))
    constexpr auto extension_pass = make_pass_test<extension>();
    constexpr auto extension_fail = make_unpass_test<extension>();

    static_assert(extension_pass("0-abc"_span));      // 数字singleton保留
    static_assert(extension_pass("a-12ab"_span));     // 合法扩展
    static_assert(extension_pass("b-12345678"_span)); // 最长扩展
    static_assert(!extension_fail("a-a"_span));       // 过短内容

    return 0;
}
// NOLINTEND