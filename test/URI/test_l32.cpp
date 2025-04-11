
#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

using OCTET = mcs::ABNF::OCTET;
constexpr auto make_span = [](auto &&arr) constexpr {
    return std::span<const OCTET>{arr};
};

int main()
{
// ------------------------- ls32 有效案例测试 -------------------------
// ls32 = h16 ":" h16 或 IPv4地址
// 辅助宏定义
#define LS32_TEST(arr, expect) \
    static_assert(mcs::ABNF::URI::ls32(make_span(arr)) == expect, #arr)

    // 案例组1：合法IPv4格式
    constexpr OCTET ipv4_valid1[] = {'1', '.', '2', '.', '3', '.', '4'}; // 1.2.3.4
    constexpr OCTET ipv4_valid2[] = {
        '2', '5', '5', '.', '2', '5', '5', '.',
        '2', '5', '5', '.', '2', '5', '5'}; // 255.255.255.255
    constexpr OCTET ipv4_valid3[] = {'0', '.', '0', '.', '0', '.', '0'}; // 0.0.0.0
    constexpr OCTET ipv4_valid4[] = {'1', '2', '7', '.', '0',
                                     '.', '0', '.', '1'}; // 127.0.0.1
    LS32_TEST(ipv4_valid1, true);
    LS32_TEST(ipv4_valid2, true);
    LS32_TEST(ipv4_valid3, true);
    LS32_TEST(ipv4_valid4, true);

    // 案例组2：合法h16:h16格式
    constexpr OCTET h16_valid1[] = {'a', 'b', 'c', 'd', ':',
                                    '1', '2', '3', '4'}; // abcd:1234
    constexpr OCTET h16_valid2[] = {'F', 'F', 'F', 'F', ':',
                                    'F', 'F', 'F', 'F'}; // FFFF:FFFF
    constexpr OCTET h16_valid3[] = {'0', ':', '0'};      // 0:0
    constexpr OCTET h16_valid4[] = {'1', '2', ':', '3',
                                    '4', '5', '6'}; // 12:3456（前导零省略）
    LS32_TEST(h16_valid1, true);
    LS32_TEST(h16_valid2, true);
    LS32_TEST(h16_valid3, true);
    LS32_TEST(h16_valid4, true);

    // ------------------------- ls32 无效案例测试 -------------------------
    // 无效IPv4格式
    constexpr OCTET ipv4_invalid1[] = {'2', '5', '6', '.', '0',
                                       '.', '0', '.', '1'}; // 256.0.0.1
    constexpr OCTET ipv4_invalid2[] = {'1', '2', '3', '.', '4', '5',
                                       '.', '6', '.', '7', '8', '9'};      // 123.45.6.789
    constexpr OCTET ipv4_invalid3[] = {'1', '.', '2', '.', '3'};           // 不完整地址
    constexpr OCTET ipv4_invalid4[] = {'a', '.', 'b', '.', 'c', '.', 'd'}; // 非数字字符
    LS32_TEST(ipv4_invalid1, false);
    LS32_TEST(ipv4_invalid2, false);
    LS32_TEST(ipv4_invalid3, false);
    LS32_TEST(ipv4_invalid4, false);

    // 无效h16:h16格式
    constexpr OCTET h16_invalid1[] = {'1', '2', '3', '4', '5',
                                      ':', 'a', 'b', 'c'}; // 前段超过4字符
    constexpr OCTET h16_invalid2[] = {'g', 'h', 'i', ':',
                                      '1', '2', '3'}; // 非法十六进制字符
    constexpr OCTET h16_invalid3[] = {'a', 'b', ':', 'c', 'd', ':', 'e', 'f'}; // 多冒号
    constexpr OCTET h16_invalid4[] = {'a', 'b', 'c', 'd', 'e', 'f'};           // 无冒号
    LS32_TEST(h16_invalid1, false);
    LS32_TEST(h16_invalid2, false);
    LS32_TEST(h16_invalid3, false);
    LS32_TEST(h16_invalid4, false);

    // 混合格式错误
    constexpr OCTET mix_invalid1[] = {'1', '.', '2', ':', '3', '.', '4'}; // 1.2:3.4
    constexpr OCTET mix_invalid2[] = {'a', '.', 'b', ':', 'c', '.', 'd'}; // a.b:c.d
    constexpr OCTET mix_invalid3[] = {'1', ':', '2', '.', '3', ':', '4'}; // 1:2.3:4
    LS32_TEST(mix_invalid1, false);
    LS32_TEST(mix_invalid2, false);
    LS32_TEST(mix_invalid3, false);

    // ------------------------- 边界条件测试 -------------------------
    // 最短合法格式
    constexpr OCTET min_valid1[] = {'0', ':', '0'};                     // 0:0
    constexpr OCTET min_valid2[] = {'0', '.', '0', '.', '0', '.', '0'}; // 0.0.0.0
    LS32_TEST(min_valid1, true);
    LS32_TEST(min_valid2, true);

    // 最长合法格式
    constexpr OCTET max_valid1[] = {'f', 'f', 'f', 'f', ':',
                                    'f', 'f', 'f', 'f'}; // ffff:ffff (9字节)
    constexpr OCTET max_valid2[] = {'2', '5', '5', '.', '2', '5', '5', '.',
                                    '2', '5', '5', '.', '2', '5', '5'}; // 15字节
    LS32_TEST(max_valid1, true);
    LS32_TEST(max_valid2, true);

    // 格式异常测试
    constexpr OCTET edge_invalid1[] = {':', '1', '2', '3', '4'}; // :1234（缺少前段）
    constexpr OCTET edge_invalid2[] = {'1', '2', '3', '4', ':'}; // 1234:（缺少后段）
    constexpr OCTET edge_invalid3[] = {'.', '1', '.', '2',
                                       '.', '3', '.', '4'}; // .1.2.3.4
    LS32_TEST(edge_invalid1, false);
    LS32_TEST(edge_invalid2, false);
    LS32_TEST(edge_invalid3, false);
    return 0;
}

// NOLINTEND