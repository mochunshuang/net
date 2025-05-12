#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // challenge = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS "," OWS auth-param )
    // ] ) ]
    constexpr auto challenge_pass = make_pass_test<mcs::abnf::http::challenge>();
    constexpr auto challenge_fail = make_unpass_test<mcs::abnf::http::challenge>();

    // 有效challenge测试
    static_assert(challenge_pass("Basic"_span));         // 仅auth-scheme
    static_assert(challenge_pass("Bearer abc123"_span)); // 带token68

    // 无效challenge测试
    static_assert(challenge_fail("Digest param=value"_span));     // 单个auth-param
    static_assert(challenge_fail("OAuth a=1, b=2"_span));         // 多个auth-param
    static_assert(challenge_fail("Custom , , param=empty"_span)); // 空白OWS处理
    static_assert(challenge_fail("invalid@scheme"_span));         // 非法scheme字符
    static_assert(challenge_fail("Basic; param=1"_span));         // 错误分隔符
    static_assert(challenge_fail("OAuth a==1"_span));             // 非法param格式
    static_assert(challenge_fail("Bearer abc\x01"_span));         // 控制字符

    // NOTE: 空格 和 等号，不是随便都能填写的
    {
        static_assert(not make_unpass_test<mcs::abnf::http::auth_param>()("="_span));
        static_assert(not make_unpass_test<mcs::abnf::http::token68>()("="_span));
        static_assert(not make_unpass_test<mcs::abnf::http::auth_param>()(" "_span));
        static_assert(not make_unpass_test<mcs::abnf::http::token68>()(" "_span));
    }

    return 0;
}
// NOLINTEND