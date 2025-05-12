#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // expectation = token [ "=" ( token / quoted-string ) parameters ]
    constexpr auto expectation = make_pass_test<mcs::abnf::http::expectation>();
    constexpr auto not_expectation = make_unpass_test<mcs::abnf::http::expectation>();

    // 有效 expectation 测试
    static_assert(expectation("100-continue"_span));   // 标准期望值
    static_assert(expectation("custom=123"_span));     // 带token参数
    static_assert(expectation("name=\"value\""_span)); // 带quoted-string参数

    // 无效 expectation 测试
    static_assert(not not_expectation("=value"_span));      // 缺少token
    static_assert(not_expectation("name=unclosed\""_span)); // 引号未闭合
    static_assert(not_expectation("invalid@key"_span));     // 非法token字符
    static_assert(not_expectation("key=val;param=)"_span)); // 非法参数符号
    static_assert(not_expectation("ext;param=abc"_span));   // 带参数列表
    {
        static_assert(make_unpass_test<mcs::abnf::http::token>()("ext;param"_span));
    }
    return 0;
}
// NOLINTEND