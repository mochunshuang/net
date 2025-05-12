#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    /**
     * challenge = auth-scheme [ 1*SP ( token68 / [ auth-param *( OWS ","
                   OWS auth-param ) ] ) ]
     */
    // auth-scheme = token
    // WWW-Authenticate = [ challenge *( OWS "," OWS challenge ) ]
    constexpr auto www_auth_pass = make_pass_test<mcs::abnf::http::WWW_Authenticate>();
    constexpr auto www_auth_fail = make_unpass_test<mcs::abnf::http::WWW_Authenticate>();

    static_assert(www_auth_pass("Basicrealm"_span));               // 单个challenge
    static_assert(www_auth_pass("Basicrealm      "_span));         // 单个challenge
    static_assert(www_auth_pass("Basicrealm      "_span));         // 单个challenge
    static_assert(www_auth_pass("Basic key=value"_span));          // 单个challenge
    static_assert(www_auth_pass("Bearer, Digest nonce=abc"_span)); // 多challenge

    static_assert(www_auth_pass("Basicrealm  , ada"_span)); // 多个challenge
    {
        // NOTE: challenge 要调整  alternative 的顺序
        static_assert(make_pass_test<mcs::abnf::http::auth_param>()("key=value"_span));
        static_assert(make_unpass_test<mcs::abnf::http::token68>()("key=value"_span));

        using Rule =
            optional<sequence<auth_param,
                              zero_or_more<sequence<OWS, Char<','>, OWS, auth_param>>>>;
        static_assert(make_pass_test<Rule>()("key=value"_span));
    }

    // NOTE: token 不允许 空格 和 =

    static_assert(www_auth_fail("  Newauth  ,  OAuth error=invalid"_span)); // 带OWS
    static_assert(www_auth_fail("Basic,,"_span));                // 空challenge项
    static_assert(www_auth_fail("Invalid@Scheme param=1"_span)); // 非法scheme

    // NOTE:  auth-scheme = token, token 和 token68 有重叠，要调整顺序
    static_assert(www_auth_fail("Basicrealm    ada"_span)); // 单个challenge

    return 0;
}
// NOLINTEND