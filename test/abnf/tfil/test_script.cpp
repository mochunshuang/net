#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    //  script        = 4ALPHA              ; ISO 15924 code
    constexpr auto script_pass = make_pass_test<script>();
    constexpr auto script_fail = make_unpass_test<script>();
    static_assert(script_pass("Latn"_span)); // 4字母

    static_assert(!script_fail("L2n"_span));  // 包含数字
    static_assert(script_fail("Latin"_span)); // 过长

    return 0;
}
// NOLINTEND