#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Proxy-Authentication-Info = [ auth-param *( OWS "," OWS auth-param ) ]
    constexpr auto proxy_auth_info_pass =
        make_pass_test<mcs::abnf::http::Proxy_Authorization_Info>();
    constexpr auto proxy_auth_info_fail =
        make_unpass_test<mcs::abnf::http::Proxy_Authorization_Info>();

    // 有效Proxy-Authentication-Info测试
    static_assert(proxy_auth_info_pass("nc=00000001"_span));             // 单个参数
    static_assert(proxy_auth_info_pass("rspauth=foo, cnonce=bar"_span)); // 多参数
    static_assert(proxy_auth_info_pass("nextnonce=  \"quoted\""_span));  // 带OWS和引号
    static_assert(proxy_auth_info_pass("key=1, key=2"_span));            // 重复键

    // 无效Proxy-Authentication-Info测试
    static_assert(proxy_auth_info_fail("=value"_span));         // 空键
    static_assert(proxy_auth_info_fail("key=unclosed\""_span)); // 引号未闭合
    static_assert(proxy_auth_info_fail("bad key=val"_span));    // 非法键名
    static_assert(proxy_auth_info_fail("key=,val"_span));       // 空值

    static_assert(proxy_auth_info_fail("nextnonce=  \"quoted\"  "_span)); // 尾带OWS

    return 0;
}
// NOLINTEND