

#include "../test_head.hpp"

#include <array>
#include <cassert>
#include <span>
#include <utility>

// NOLINTBEGIN
using OCTET = mcs::abnf::OCTET;
constexpr auto make_span2 = [](auto &&arr) constexpr {
    return std::span<const OCTET>{std::forward<decltype(arr)>(arr)};
};

int main()
{
    using namespace mcs::abnf::uri; // NOLINT

    // ------------------------- 有效IPv6地址测试 -------------------------
    // Rule 1: 6(h16 ":") ls32 (完整格式)
    // 有效案例：2001:db8:85a3:0:0:8a2e:370:7334
    static constexpr std::array<OCTET, 31> case1 =
        {
            '2', '0', '0', '1', ':', 'd', 'b', '8', ':', '8', '5',
            'a', '3', ':', '0', ':', '0', ':', '8', 'a', '2', 'e',
            ':', '3', '7', '0', ':', '7', '3', '3', '4' // ls32为纯IPv6格式
        };

    // NOTE: 数组太长，是不能编译期测试的
    assert(IPv6address(case1));
    // static_assert(IPv6address(make_span2(case1)));
    // Rule 2: "::" 5(h16 ":") ls32 (双冒号开头)
    // 有效案例：::1:2:3:4:5:6.7.8.9
    static constexpr OCTET case2[] = {
        ':', ':', '1', ':', '2', ':', '3', ':', '4', ':',
        '5', ':', '6', '.', '7', '.', '8', '.', '9' // ls32为IPv4格式
    };
    static_assert(IPv6address(make_span2(case2)), "Rule 2失败");

    // Rule 3: [h16] "::" 4(h16 ":") ls32 (可选前缀+双冒号)
    // 有效案例1：a::b:c:d:e:fffg:1234
    static constexpr OCTET case3a[] = {
        'a', ':', ':', 'b', ':', 'c', ':', 'd', ':', 'e',
        ':', 'f', 'f', 'f', 'f', ':', '1', '2', '3', '4' // ls32为IPv6格式
    };
    // 有效案例2：::b:c:d:e:fff:1234
    static constexpr OCTET case3b[] = {':', ':', 'b', ':', 'c', ':', 'd', ':', 'e',
                                       ':', 'f', 'f', 'f', ':', '1', '2', '3', '4'};
    static_assert(IPv6address(make_span2(case3a)), "Rule 3案例1失败");
    static_assert(IPv6address(make_span2(case3b)), "Rule 3案例2失败");

    // Rule 4: [*1(h16 ":") h16] "::" 3(h16 ":") ls32 (最多1个前缀段)
    // 有效案例：a:b::c:d:e:1.2.3.4
    static constexpr OCTET case4[] = {
        'a', ':', 'b', ':', ':', 'c', ':', 'd', ':',
        'e', ':', '1', '.', '2', '.', '3', '.', '4' // IPv4结尾
    };
    static_assert(IPv6address(make_span2(case4)), "Rule 4失败");

    // Rule 5: [*2(h16 ":") h16] "::" 2(h16 ":") ls32 (最多2个前缀段)
    // 有效案例：1:2:3::4:5:6.7.8.9
    static constexpr OCTET case5[] = {'1', ':', '2', ':', '3', ':', ':', '4', ':',
                                      '5', ':', '6', '.', '7', '.', '8', '.', '9'};
    static_assert(IPv6address(make_span2(case5)), "Rule 5失败");

    // Rule 6: [*3(h16 ":") h16] "::" h16 ":" ls32 (最多3个前缀段)
    // 有效案例：a:b:c:d::e:ffff:1234
    static constexpr OCTET case6[] = {'a', ':', 'b', ':', 'c', ':', 'd', ':', ':', 'e',
                                      ':', 'f', 'f', 'f', 'f', ':', '1', '2', '3', '4'};
    static_assert(IPv6address(make_span2(case6)), "Rule 6失败");

    // Rule 7: [*4(h16 ":") h16] "::" ls32 (最多4个前缀段)
    // 有效案例：1:2:3:4:5::6.7.8.9
    static constexpr OCTET case7[] = {'1', ':', '2', ':', '3', ':', '4', ':', '5',
                                      ':', ':', '6', '.', '7', '.', '8', '.', '9'};
    static_assert(IPv6address(make_span2(case7)), "Rule 7失败");

    // Rule 8: [*5(h16 ":") h16] "::" h16 (最多5个前缀段)
    // 有效案例：1:2:3:4:5:6::7
    static constexpr OCTET case8[] = {
        '1', ':', '2', ':', '3', ':', '4',
        ':', '5', ':', '6', ':', ':', '7' // 双冒号在末尾
    };
    static_assert(IPv6address(make_span2(case8)), "Rule 8失败");

    // Rule 9: [*6(h16 ":") h16] "::" (最多6个前缀段)
    // 有效案例：1:2:3:4:5:6:7::
    static constexpr OCTET case9[] = {'1', ':', '2', ':', '3', ':', '4', ':',
                                      '5', ':', '6', ':', '7', ':', ':'};
    static_assert(IPv6address(make_span2(case9)), "Rule 9失败");

    // ------------------------- 特殊边界测试 -------------------------
    // 最短合法地址 ::
    static constexpr OCTET case_min[] = {':', ':'};
    static_assert(IPv6address(make_span2(case_min)), "最短地址失败");

    // 混合大小写测试（规范要求不区分大小写）
    static constexpr OCTET case_mixed_case[] = {'F', 'F', 'F', 'F', ':',
                                                ':', 'A', 'B', 'C', 'D'}; // ::ABCD
    static_assert(IPv6address(make_span2(case_mixed_case)), "大小写敏感问题");

    // 最大长度测试（8段全展开）
    static constexpr OCTET case_full[] = {
        '0', ':', '0', ':', '0', ':', '0', ':',
        '0', ':', '0', ':', '0', ':', '0'}; // :: 的等效形式
    // static_assert(IPv6address(make_span2(case_full)), "全零地址失败");
    assert(IPv6address(make_span2(case_full))); // NOTE: 长数组只能运行时
    static_assert(detail::check_common_tail(case_full, 6));

    // 非法双冒号测试（包含多个::）
    static constexpr OCTET invalid_double_colon[] = {'1', ':', ':', '2', ':', ':', '3'};
    static_assert(!IPv6address(make_span2(invalid_double_colon)), "应拒绝多个双冒号");
    return 0;
}

// NOLINTEND