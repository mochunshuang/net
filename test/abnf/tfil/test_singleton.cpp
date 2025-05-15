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
    constexpr auto singleton_pass = make_pass_test<singleton>();
    constexpr auto singleton_fail = make_unpass_test<singleton>();
    static_assert(singleton_pass("A"_span)); // A-W
    static_assert(singleton_pass("Y"_span)); // Y-Z
    static_assert(singleton_pass("d"_span)); // a-w
    static_assert(singleton_pass("9"_span)); // 数字

    static_assert(!singleton_fail("X"_span)); // 排除X
    static_assert(!singleton_fail("x"_span)); // 排除x

    return 0;
}
// NOLINTEND