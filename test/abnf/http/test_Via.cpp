#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Via = [ ( received-protocol RWS received-by [ RWS comment ] ) *( OWS "," OWS ... )
    // ]
    // received-protocol = [ protocol-name "/" ] protocol-version
    constexpr auto via_pass = make_pass_test<mcs::abnf::http::Via>();
    constexpr auto via_fail = make_unpass_test<mcs::abnf::http::Via>();

    // 有效Via测试
    static_assert(via_pass("1.1 proxy.example.com"_span));          // 基础格式
    static_assert(via_pass("HTTP/1.1 gateway (Test Server)"_span)); // 带注释
    static_assert(via_pass("1.0 _secret-proxy_"_span));             // 带下划线

    static_assert(via_pass("HTTP|1.1 bad_proxy"_span)); // 非法协议
    {
        static_assert(
            make_pass_test<mcs::abnf::http::received_protocol>()("HTTP|1.1"_span));
        static_assert(make_pass_test<mcs::abnf::http::received_by>()("bad_proxy"_span));
    }

    // 无效Via测试
    static_assert(via_fail("2.0 [2001:db8::1], 1.0 hidden"_span)); // 多条目
    static_assert(via_fail("1.1 proxy.example.com, "_span));       // 空尾随条目

    static_assert(via_fail("1.1 \"quoted\""_span));     // 错误接收者格式
    static_assert(via_fail("1.1 host (unclosed"_span)); // 未闭合注释

    return 0;
}
// NOLINTEND