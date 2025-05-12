#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // acceptable-ranges = range-unit *( OWS "," OWS range-unit )
    constexpr auto acceptable_ranges_pass =
        make_pass_test<mcs::abnf::http::acceptable_ranges>();
    constexpr auto acceptable_ranges_fail =
        make_unpass_test<mcs::abnf::http::acceptable_ranges>();

    // 有效ranges测试
    static_assert(acceptable_ranges_pass("bytes"_span));          // 单个range-unit
    static_assert(acceptable_ranges_pass("items,bytes"_span));    // 简单分隔
    static_assert(acceptable_ranges_pass("none  ,  pages"_span)); // 带OWS
    static_assert(acceptable_ranges_pass("custom-unit"_span));    // 自定义unit

    // 无效ranges测试
    static_assert(acceptable_ranges_fail("bytes;items"_span));  // 错误分隔符
    static_assert(acceptable_ranges_fail("bytes, "_span));      // 空尾随unit
    static_assert(acceptable_ranges_fail("unit1,unit2,"_span)); // 结尾逗号
    static_assert(acceptable_ranges_fail("invalid unit"_span)); // 含空格unit

    return 0;
}
// NOLINTEND