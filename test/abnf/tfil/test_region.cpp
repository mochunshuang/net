#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     * region        = 2ALPHA              ; ISO 3166-1 code
                     / 3DIGIT              ; UN M.49 code
     *
     */
    constexpr auto region_pass = make_pass_test<region>();
    constexpr auto region_fail = make_unpass_test<region>();
    static_assert(region_pass("US"_span));  // 2字母代码
    static_assert(region_pass("419"_span)); // 3数字代码

    static_assert(!region_fail("0A"_span));  // 混合类型
    static_assert(region_fail("1234"_span)); // 过长

    return 0;
}
// NOLINTEND