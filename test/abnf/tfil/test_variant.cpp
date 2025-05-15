#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::tfil;

int main()
{
    /**
      * variant       = 5*8alphanum         ; registered variants
                      / (DIGIT 3alphanum)
      *
      */
    constexpr auto variant_pass = make_pass_test<variant>();
    constexpr auto variant_fail = make_unpass_test<variant>();
    static_assert(variant_pass("1996"_span));     // 数字开头
    static_assert(variant_pass("rozaj"_span));    // 5字母
    static_assert(variant_pass("12345678"_span)); // 8字符
    static_assert(variant_pass("abcdefgh"_span)); // 8字符
    static_assert(variant_pass("1234"_span));     // (DIGIT 3alphanum)

    static_assert(!variant_fail("a234"_span));  // 不满足第二个
    static_assert(!variant_fail("abcd!"_span)); // 非法字符

    return 0;
}
// NOLINTEND