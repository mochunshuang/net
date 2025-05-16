#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // auth-param = token BWS "=" BWS ( token / quoted-string )
    // token = 1*tchar
    constexpr auto auth_param_pass = make_pass_test<mcs::abnf::http::auth_param>();
    constexpr auto auth_param_fail = make_unpass_test<mcs::abnf::http::auth_param>();

    // 有效auth参数测试
    static_assert(auth_param_pass("key=value"_span));           // 简单键值对
    static_assert(auth_param_pass("token = \"quoted\""_span));  // 带空白和引号
    static_assert(auth_param_pass("complex = a%20value"_span)); // 编码值

    static_assert(auth_param_pass("q=1"_span)); // 简单键值对

    // 无效auth参数测试
    static_assert(not auth_param_fail("empty= "_span));     // 空值
    static_assert(not auth_param_fail("=value"_span));      // 缺少key
    static_assert(auth_param_fail("key=unclosed\""_span));  // 引号未闭合
    static_assert(not auth_param_fail("bad key=val"_span)); // key含空格
    static_assert(auth_param_fail("key=bad;value"_span));   // 非法字符

    return 0;
}
// NOLINTEND