#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    /**
     * credentials = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS ","
                                 OWS auth-param ) ] ) ]
     *
     */
    // Proxy-Authorization = credentials
    constexpr auto proxy_authz_pass =
        make_pass_test<mcs::abnf::http::Proxy_Authorization>();
    constexpr auto proxy_authz_fail =
        make_unpass_test<mcs::abnf::http::Proxy_Authorization>();

    // 有效Proxy-Authorization测试
    static_assert(proxy_authz_pass("Basic dXNlcjpwYXNz"_span));     // Base64凭证 token68
    static_assert(proxy_authz_pass("Bearer token123"_span));        // Bearer令牌 token68
    static_assert(proxy_authz_pass("Digest username======="_span)); // token68
    // static_assert(proxy_authz_pass("Custom scheme=\"quoted\""_span)); // 引号值
    {
        // scheme=\"quoted\"
        static_assert(
            make_pass_test<mcs::abnf::http::auth_param>()("scheme=\"quoted\""_span));

        constexpr auto ret = make_rule_test<mcs::abnf::http::Proxy_Authorization>(
            "Custom scheme=\"quoted\""_span);
        static_assert(ret.second.cur_index == "Custom scheme="_span.size());
        // NOTE: 解决的办法是 = 后面，断言。怎么兼容呢？
        // TODO 修改 token68的顺序，或许是错误的
        // TODO \r\n 每一行都说输入的情况下，或许能以此来兼容。token68 必须 ctx.done 结束
    }
    // 无效Proxy-Authorization测试
    static_assert(proxy_authz_fail("Basic: credentials"_span));     // 错误分隔符
    static_assert(proxy_authz_fail("Bearer token=1,2"_span));       // 非法参数格式
    static_assert(proxy_authz_fail("Invalid Scheme param=1"_span)); // 非法scheme
    static_assert(proxy_authz_fail("Basic\x01Credentials"_span));   // 控制字符

    return 0;
}
// NOLINTEND