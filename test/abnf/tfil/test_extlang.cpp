#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
     *  extlang       = 3ALPHA              ; selected ISO 639 codes
     *                  *2("-" 3ALPHA)      ; permanently reserved
     *
     */
    constexpr auto extlang_pass = make_pass_test<extlang>();
    constexpr auto extlang_fail = make_unpass_test<extlang>();
    static_assert(extlang_pass("eng"_span));         // 基础3字母
    static_assert(extlang_pass("eng-fra"_span));     // 带一个扩展
    static_assert(extlang_pass("eng-fra-spa"_span)); // 带两个扩展

    static_assert(!extlang_fail("en"_span));             // 过短
    static_assert(extlang_fail("eng-"_span));            // 空扩展
    static_assert(extlang_fail("eng-fra-spa-abc"_span)); // 带3个扩展

    return 0;
}
// NOLINTEND