

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>
#include <span>

// NOLINTBEGIN

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    ////////////////////////////////////////////
    // 有效测试用例（应通过 static_assert 验证）
    ////////////////////////////////////////////

    // IPv6 有效用例
    constexpr auto valid_ipv6_1 = make_array("[2001:db8::1]");        // 标准 IPv6 缩写
    constexpr auto valid_ipv6_2 = make_array("[::1]");                // 最小化 IPv6 地址
    constexpr auto valid_ipv6_3 = make_array("[1:2:3:4:5:6:7:8]");    // 全展开无缩写
    constexpr auto valid_ipv6_4 = make_array("[::ffff:192.168.0.1]"); // IPv4 嵌入 ls32
    constexpr auto valid_ipv6_5 =
        make_array("[A:bCd:0FF:0:0:0:0:0]");                    // 混合大小写 HEXDIG
    constexpr auto valid_ipv6_6 = make_array("[a::b:1.2.3.4]"); // 合法 ls32 格式

    // IPvFuture 有效用例
    constexpr auto valid_future_1 = make_array("[v1.a]"); // 最小合法结构
    {
        static_assert(IPvFuture(make_array("v1.a")));
    }
    constexpr auto valid_future_2 = make_array("[vF.~]"); // unreserved 字符
    constexpr auto valid_future_3 =
        make_array("[v123456.:+]"); // 长 HEXDIG + 混合合法字符
    constexpr auto valid_future_4 = make_array("[vA.!$&'()*+,;=]"); // 全 sub-delims
    constexpr auto valid_future_5 = make_array("[v7._:a~]"); // 混合 unreserved 和冒号
    {
        constexpr auto valid_ipv6_1 = make_array("2001:db8::1");
        static_assert(IPv6address(valid_ipv6_1));
        static_assert(IPv6address(valid_ipv6_1));

        constexpr auto valid_ipv6_3 = make_array("1:2:3:4:5:6:7:8"); // 全展开无缩写
        assert(IPv6address(valid_ipv6_3));
    }

    ////////////////////////////////////////////
    // 无效测试用例（应不通过验证）
    ////////////////////////////////////////////

    // 格式错误（不匹配方括号）
    constexpr auto invalid_fmt_1 = make_array("2001:db8::1");  // 缺少方括号
    constexpr auto invalid_fmt_2 = make_array("[2001:db8::1"); // 缺少右括号
    constexpr auto invalid_fmt_3 = make_array("2001:db8::1]"); // 缺少左括号
    constexpr auto invalid_fmt_4 = make_array("[]");           // 空内容

    // IPv6 无效用例
    constexpr auto invalid_ipv6_1 = make_array("[:::1]");              // 连续三个冒号
    constexpr auto invalid_ipv6_2 = make_array("[1:2:3:4:5:6:7:8:9]"); // 超过 8 个 h16
    constexpr auto invalid_ipv6_3 = make_array("[::g]");               // 非法 HEXDIG 'g'
    constexpr auto invalid_ipv6_4 = make_array("[1::2::3]");           // 多个双冒号缩写
    constexpr auto invalid_ipv6_5 = make_array("[::1.2.3.256]");       // 非法 IPv4 地址

    // IPvFuture 无效用例
    constexpr auto invalid_future_1 = make_array("[vg.x]");      // 'g' 非 HEXDIG
    constexpr auto invalid_future_2 = make_array("[v1.]");       // 尾部为空
    constexpr auto invalid_future_3 = make_array("[v1.#]");      // gen-delims '#'
    constexpr auto invalid_future_4 = make_array("[v1. /]");     // 包含 '/' 空格
    constexpr auto invalid_future_5 = make_array("[v123.abc%]"); // 非法字符 '%'
    {
        // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
        static_assert(not HEXDIG('g'));
        static_assert(not IPvFuture(make_array("vg.x")));
        static_assert(not IPvFuture(make_array("v1.")));
    }

    // 混合类型错误
    constexpr auto valid_mix_1 = make_array("[v1.::1]"); // 混合 IPvFuture 和 IPv6
    {
        static_assert(IPvFuture(make_array("v1.::1"))); // 是有效的
    }
    constexpr auto invalid_mix_2 = make_array("[2001:db8:v1.a]"); // IPv6 中出现 'v'

    ////////////////////////////////////////////
    // 静态断言验证
    ////////////////////////////////////////////

    // 有效用例断言
    static_assert(IP_literal(valid_ipv6_1));
    static_assert(IP_literal(valid_ipv6_2));
    assert(IP_literal(valid_ipv6_3));
    static_assert(IP_literal(valid_ipv6_4));
    // NOTE: 分支一多无法编译期检查。总之编译期一定的，只能是 consteavl
    assert(IP_literal(valid_ipv6_5));
    assert(IP_literal(valid_ipv6_6));
    assert(IP_literal(valid_future_1));
    assert(IP_literal(valid_future_2));
    assert(IP_literal(valid_future_3));
    assert(IP_literal(valid_future_4));
    assert(IP_literal(valid_future_5));

    assert(IP_literal(valid_mix_1));

    // 无效用例断言
    assert(!IP_literal(invalid_fmt_1));
    assert(!IP_literal(invalid_fmt_2));
    assert(!IP_literal(invalid_fmt_3));
    assert(!IP_literal(invalid_fmt_4));
    assert(!IP_literal(invalid_ipv6_1));
    assert(!IP_literal(invalid_ipv6_2));
    assert(!IP_literal(invalid_ipv6_3));
    assert(!IP_literal(invalid_ipv6_4));
    assert(!IP_literal(invalid_ipv6_5));
    assert(!IP_literal(invalid_future_1));
    assert(!IP_literal(invalid_future_2));
    assert(!IP_literal(invalid_future_3));
    assert(!IP_literal(invalid_future_4));
    assert(!IP_literal(invalid_future_5));

    assert(!IP_literal(invalid_mix_2));

    return 0;
}

// NOLINTEND