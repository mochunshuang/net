#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // User-Agent = product *( RWS ( product / comment ) )
    // product = token [ "/" product-version ]
    constexpr auto user_agent_pass = make_pass_test<mcs::abnf::http::User_Agent>();
    constexpr auto user_agent_fail = make_unpass_test<mcs::abnf::http::User_Agent>();

    // 有效User-Agent测试
    static_assert(user_agent_pass("Mozilla/5.0"_span)); // 单个product
    static_assert(
        user_agent_pass("CERN-LineMode/2.15 (libwww-FM/2.14)"_span)); // 混合格式
    static_assert(user_agent_pass("MyApp/1.0.0  (Linux; x64)"_span)); // 带注释
    static_assert(user_agent_pass("Foo/1 Bar/2 (Test)"_span));        // 多个product

    // 无效User-Agent测试
    static_assert(user_agent_fail("Product/1.0(no space)"_span)); // 缺少RWS
    static_assert(user_agent_fail("Invalid/Version@1"_span));     // 非法字符
    static_assert(user_agent_fail("Mozilla/5.0 \x01"_span));      // 控制字符
    static_assert(not user_agent_fail("  "_span));                // 纯空白
    return 0;
}
// NOLINTEND