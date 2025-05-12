#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Vary = [ ( "*" / field-name ) *( OWS "," OWS ( "*" / field-name ) ) ]
    constexpr auto vary_pass = make_pass_test<mcs::abnf::http::Vary>();
    constexpr auto vary_fail = make_unpass_test<mcs::abnf::http::Vary>();

    // 有效Vary测试
    static_assert(vary_pass("*"_span));                           // 单独星号
    static_assert(vary_pass("User-Agent, Accept-Language"_span)); // 字段列表
    static_assert(vary_pass("Accept-Encoding"_span));             // 单个字段
    static_assert(vary_pass("*  ,  Content-Type"_span));          // 混合类型

    // 无效Vary测试
    static_assert(vary_fail("*,invalid field"_span)); // 非法字段名
    static_assert(vary_fail("User Agent"_span));      // 含空格字段
    static_assert(vary_fail("**, Accept"_span));      // 重复星号
    static_assert(vary_fail(",,,"_span));             // 空列表项

    // NOTE: 空格不能随便
    static_assert(vary_fail("  *  ,  Content-Type  "_span)); // 混合类型

    return 0;
}
// NOLINTEND