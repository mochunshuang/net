

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <array>
#include <cassert>
#include <span>

// NOLINTBEGIN

#include <array>

#include <array>
#include <cstdint>

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= IP-literal 有效用例 =================
    constexpr auto ipv6_1 = "[2001:db8::1]"_span;          // 标准 IPv6 缩写
    constexpr auto ipv6_2 = "[::ffff:192.168.0.1]"_span;   // IPv4 嵌入格式
    constexpr auto ipv6_3 = "[1:2:3:4:5:6:7:8]"_span;      // 全展开 IPv6
    constexpr auto ip_future_1 = "[v1.a]"_span;            // 最小 IPvFuture
    constexpr auto ip_future_2 = "[vF.~!$&'()*+,;=]"_span; // 全部允许字符
    static_assert(host(ipv6_1));
    static_assert(host(ipv6_2));

    static_assert(ipv6_3[1] == '1');
    static_assert(ipv6_3[15] == '8');
    static_assert(ipv6_3[16] == ']');
    // static_assert(host(ipv6_3)); // NOTE: 但是 不是常量表达式,因为 span
    EXPECT(host(ipv6_3));
    EXPECT(host(ip_future_1));
    EXPECT(host(ip_future_2));

    // static_assert(host(auto("[1:2:3:4:5:6:7:8]"_span)));
    // static_assert(host("[v1.a]"_span));
    // static_assert(host(std::array<OCTET, 17>{'[', '1', ':', '2', ':', '3', ':', '4',
    // ':',
    //                                          '5', ':', '6', ':', '7', ':', '8', ']'}));
    {
        constexpr std::array<OCTET, 17> arr{'[', '1', ':', '2', ':', '3', ':', '4', ':',
                                            '5', ':', '6', ':', '7', ':', '8', ']'};
        static_assert(arr[16] == ']');
        // NOTE: span 常量表达式很难
        //  constexpr std::span<OCTET, 17> sp(arr);
        //  constexpr std::span<const OCTET, 17> sp(arr);
        std::span<const OCTET, 17> sp(arr); // 移除 constexpr
        // static_assert(host(arr));
        // static_assert(host(sp));
        {
            // constexpr OCTET arr[] = "1234";
            // constexpr std::span sp(arr, 4);
        }
    }

    //================= IPv4address 有效用例 =================
    constexpr auto ipv4_1 = "192.168.0.1"_span;     // 标准 IPv4
    constexpr auto ipv4_2 = "255.255.255.255"_span; // 最大值边界
    constexpr auto ipv4_3 = "0.0.0.0"_span;         // 最小值边界
    constexpr auto ipv4_4 = "123.45.67.89"_span;    // 混合数值
    EXPECT(host(ipv4_1));
    EXPECT(host(ipv4_2));
    EXPECT(host(ipv4_3));
    EXPECT(host(ipv4_4));

    //================= reg-name 有效用例 ===================
    constexpr auto reg_1 = "example.com"_span;    // 常规域名
    constexpr auto reg_2 = "user_name~"_span;     // 包含 unreserved
    constexpr auto reg_3 = "!$&'()*+,;=abc"_span; // 全 sub-delims 开头
    constexpr auto reg_4 = "%20%3A%2F"_span;      // 合法 pct-encoded
    constexpr auto reg_5 = "a.b_c-d~"_span;       // 混合 unreserved

    EXPECT(host(reg_1));
    EXPECT(host(reg_2));
    EXPECT(host(reg_3));
    EXPECT(host(reg_4));
    EXPECT(host(reg_5));

    return 0;
}

// NOLINTEND