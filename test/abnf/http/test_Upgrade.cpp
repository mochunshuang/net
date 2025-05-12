#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Upgrade = [ protocol *( OWS "," OWS protocol ) ]
    // protocol = protocol-name [ "/" protocol-version ]
    constexpr auto upgrade_pass = make_pass_test<mcs::abnf::http::Upgrade>();
    constexpr auto upgrade_fail = make_unpass_test<mcs::abnf::http::Upgrade>();

    // 有效Upgrade测试
    static_assert(upgrade_pass("HTTP/2.0"_span));       // 单个协议
    static_assert(upgrade_pass("h2c, websocket"_span)); // 简单列表

    static_assert(upgrade_pass("custom_protocol/v1"_span)); // 自定义协议
    static_assert(upgrade_pass(""_span));                   // 空值（允许但需特殊处理）

    // 无效Upgrade测试
    static_assert(upgrade_fail("http/2.0,,"_span));       // 空列表项
    static_assert(upgrade_fail("h2c;websocket"_span));    // 错误分隔符
    static_assert(upgrade_fail("invalid@protocol"_span)); // 非法字符

    static_assert(upgrade_fail("  TLS/1.3  ,  SPDY/3  "_span)); // 带OWS
    {
        static_assert(
            not make_unpass_test<mcs::abnf::http::protocol_name>()("  TLS"_span));
    }

    return 0;
}
// NOLINTEND