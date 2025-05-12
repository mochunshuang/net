#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Proxy-Authenticate = [ challenge *( OWS "," OWS challenge ) ]
    constexpr auto proxy_auth_pass =
        make_pass_test<mcs::abnf::http::Proxy_Authenticate>();
    constexpr auto proxy_auth_fail =
        make_unpass_test<mcs::abnf::http::Proxy_Authenticate>();

    // 有效Proxy-Authenticate测试
    static_assert(proxy_auth_pass("Basic realm=\"Proxy\""_span));       // 单个challenge
    static_assert(proxy_auth_pass("Bearer, Digest nonce=abc"_span));    // 多challenge
    static_assert(proxy_auth_pass("AWS4-HMAC-SHA256"_span));            // 复杂scheme
    static_assert(proxy_auth_pass("KeyAuth  ,  OAuth scope=api"_span)); // 带OWS

    // 无效Proxy-Authenticate测试
    static_assert(proxy_auth_fail("Basic,,"_span));                // 空challenge项
    static_assert(proxy_auth_fail("Invalid@Scheme param=1"_span)); // 非法scheme
    static_assert(proxy_auth_fail("Bearer token=abc;def"_span));   // 错误分隔符
    static_assert(proxy_auth_fail("Digest \x01nonce=bad"_span));   // 控制字符

    static_assert(proxy_auth_fail("  KeyAuth  ,  OAuth scope=api  "_span)); // 首尾带OWS
    static_assert(proxy_auth_fail(" KeyAuth  ,  OAuth scope=api"_span));    // 首带OWS
    static_assert(proxy_auth_fail("KeyAuth  ,  OAuth scope=api  "_span));   // 尾带OWS

    return 0;
}
// NOLINTEND