
#include "../test_abnf.hpp"
#include "./test_uri.hpp"
#include <cassert>
#include <regex>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;
#include <array>
#include <iostream>

constexpr bool is_valid_ipv6(const std::string &address)
{
    const std::regex ipv6_pattern(
        R"(^([0-9A-Fa-f]{1,4}:){7}[0-9A-Fa-f]{1,4}$|)" // 标准格式
        R"(^(([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4})?::([0-9A-Fa-f]{1,4}:){0,6}[0-9A-Fa-f]{1,4}$)" // 含双冒号
    );
    return std::regex_match(address, ipv6_pattern);
}

int main()
{
    [] {
        // 使用示例
        std::string test_addr = "FFFF::ABCD";
        bool valid = is_valid_ipv6(test_addr); // 返回 true
        assert(valid);
        {
            std::string test_addr = "1::2::3";
            bool valid = is_valid_ipv6(test_addr); // 返回 true
            assert(not valid);
        }
    }();
    constexpr auto ipv6address = [](auto ctx) constexpr {
        return mcs::abnf::uri::IPv6address{}(ctx);
    };

    // ------------------------- 有效IPv6地址测试 -------------------------
    // Rule 1: 6(h16 ":") ls32 (完整格式)
    // 有效案例：2001:db8:85a3:0:0:8a2e:370:7334
    static constexpr std::array<const OCTET, 31> case1 =
        {
            '2', '0', '0', '1', ':', 'd', 'b', '8', ':', '8', '5',
            'a', '3', ':', '0', ':', '0', ':', '8', 'a', '2', 'e',
            ':', '3', '7', '0', ':', '7', '3', '3', '4' // ls32为纯IPv6格式
        };
    {
        constexpr auto sub = std::span{case1};
        constexpr auto sub0 = sub.first(23);
        static_assert(sub0[22] == ':');
        constexpr auto h16_6 = [](auto ctx) constexpr {
            return times<6, sequence<h16, CharSensitive<':'>>>{}(ctx);
        };
        constexpr auto ls32_rule = [](auto ctx) constexpr {
            return ls32{}(ctx);
        };
        static_assert(h16_6(make_parser_ctx(sub0)));
        static_assert(ls32_rule(make_parser_ctx(sub.subspan(23))));

        // 因此 应该是 满足case_0的，为什么不满足呢？
        constexpr auto case_0 = [](auto ctx) constexpr {
            return mcs::abnf::uri::__detail::IPv6_0{}(ctx);
        };
        static_assert(case_0(detail::make_parser_ctx(std::span{case1})));
        {
            // 因为上下文信息要更新
            // Rule 1: 6(h16 ":") ls32 (完整格式)
            auto ctx = detail::make_parser_ctx(std::span{case1});
            auto h16_6 = [](auto &ctx) constexpr {
                return times<6, sequence<h16, CharSensitive<':'>>>{}(ctx);
            };
            auto ls32_rule = [](auto &ctx) constexpr {
                return ls32{}(ctx);
            };
            EXPECT(h16_6(ctx).value() == 23);
            EXPECT(ctx.cur_index == 23);

            EXPECT(ls32_rule(ctx).has_value());
            EXPECT(*ls32_rule(ctx) == std::span{case1}.size() - 23);
            EXPECT(ctx.cur_index == std::span{case1}.size());
        }
    }

    static_assert(ipv6address(detail::make_parser_ctx(std::span{case1})));
    // static_assert(ipv6address(make_parser_ctx(case1)));
    // Rule 2: "::" 5(h16 ":") ls32 (双冒号开头)
    // 有效案例：::1:2:3:4:5:6.7.8.9
    static constexpr OCTET case2[] = {
        ':', ':', '1', ':', '2', ':', '3', ':', '4', ':',
        '5', ':', '6', '.', '7', '.', '8', '.', '9' // ls32为IPv4格式
    };
    static_assert(ipv6address(make_parser_ctx(case2)), "Rule 2失败");

    // Rule 3: [h16] "::" 4(h16 ":") ls32 (可选前缀+双冒号)
    // 有效案例1：a::b:c:d:e:fffg:1234
    static constexpr OCTET case3a[] = {
        'a', ':', ':', 'b', ':', 'c', ':', 'd', ':', 'e',
        ':', 'f', 'f', 'f', 'f', ':', '1', '2', '3', '4' // ls32为IPv6格式
    };
    // 有效案例2：::b:c:d:e:fff:1234
    static constexpr OCTET case3b[] = {':', ':', 'b', ':', 'c', ':', 'd', ':', 'e',
                                       ':', 'f', 'f', 'f', ':', '1', '2', '3', '4'};
    static_assert(ipv6address(make_parser_ctx(case3a)), "Rule 3案例1失败");
    static_assert(ipv6address(make_parser_ctx(case3b)), "Rule 3案例2失败");

    // Rule 4: [*1(h16 ":") h16] "::" 3(h16 ":") ls32 (最多1个前缀段)
    // 有效案例：a:b::c:d:e:1.2.3.4
    static constexpr OCTET case4[] = {
        'a', ':', 'b', ':', ':', 'c', ':', 'd', ':',
        'e', ':', '1', '.', '2', '.', '3', '.', '4' // IPv4结尾
    };
    static_assert(ipv6address(make_parser_ctx(case4)), "Rule 4失败");

    // Rule 5: [*2(h16 ":") h16] "::" 2(h16 ":") ls32 (最多2个前缀段)
    // 有效案例：1:2:3::4:5:6.7.8.9
    static constexpr OCTET case5[] = {'1', ':', '2', ':', '3', ':', ':', '4', ':',
                                      '5', ':', '6', '.', '7', '.', '8', '.', '9'};
    static_assert(ipv6address(make_parser_ctx(case5)), "Rule 5失败");

    // Rule 6: [*3(h16 ":") h16] "::" h16 ":" ls32 (最多3个前缀段)
    // 有效案例：a:b:c:d::e:ffff:1234
    static constexpr OCTET case6[] = {'a', ':', 'b', ':', 'c', ':', 'd', ':', ':', 'e',
                                      ':', 'f', 'f', 'f', 'f', ':', '1', '2', '3', '4'};
    static_assert(ipv6address(make_parser_ctx(case6)), "Rule 6失败");

    // Rule 7: [*4(h16 ":") h16] "::" ls32 (最多4个前缀段)
    // 有效案例：1:2:3:4:5::6.7.8.9
    static constexpr OCTET case7[] = {'1', ':', '2', ':', '3', ':', '4', ':', '5',
                                      ':', ':', '6', '.', '7', '.', '8', '.', '9'};
    static_assert(ipv6address(make_parser_ctx(case7)), "Rule 7失败");

    // Rule 8: [*5(h16 ":") h16] "::" h16 (最多5个前缀段)
    // 有效案例：1:2:3:4:5:6::7
    static constexpr OCTET case8[] = {
        '1', ':', '2', ':', '3', ':', '4',
        ':', '5', ':', '6', ':', ':', '7' // 双冒号在末尾
    };
    static_assert(ipv6address(make_parser_ctx(case8)), "Rule 8失败");

    // Rule 9: [*6(h16 ":") h16] "::" (最多6个前缀段)
    // 有效案例：1:2:3:4:5:6:7::
    static constexpr OCTET case9[] = {'1', ':', '2', ':', '3', ':', '4', ':',
                                      '5', ':', '6', ':', '7', ':', ':'};
    static_assert(ipv6address(make_parser_ctx(case9)), "Rule 9失败");

    // ------------------------- 特殊边界测试 -------------------------
    // 最短合法地址 ::
    static constexpr OCTET case_min[] = {':', ':'};
    static_assert(ipv6address(make_parser_ctx(case_min)), "最短地址失败");

    // 混合大小写测试（规范要求不区分大小写）
    static constexpr OCTET case_mixed_case[] = {'F', 'F', 'F', 'F', ':',
                                                ':', 'a', 'b', 'c', 'D'};
    static_assert(ipv6address(make_parser_ctx(case_mixed_case)), "大小写敏感问题");

    // 最大长度测试（8段全展开）
    static constexpr OCTET case_full[] = {
        '0', ':', '0', ':', '0', ':', '0', ':',
        '0', ':', '0', ':', '0', ':', '0'}; // :: 的等效形式
    static_assert(ipv6address(make_parser_ctx(case_full)), "全零地址失败");
    static_assert(ipv6address(make_parser_ctx(case_full)).value() == 15, "全零地址失败");

    // 非法双冒号测试（包含多个::）
    static constexpr OCTET invalid_double_colon[] = {'1', ':', ':', '2', ':', ':', '3'};
    static_assert(ipv6address(make_parser_ctx(invalid_double_colon)), "应拒绝多个双冒号");
    {
        // NOTE: 其实是失败的。 还是有必要。分割 在验证的。才是最稳定的，parser
        static_assert(ipv6address(make_parser_ctx(invalid_double_colon)).value() != 7,
                      "应拒绝多个双冒号");
        static_assert(ipv6address(make_parser_ctx(invalid_double_colon)).value() == 4,
                      "应拒绝多个双冒号");
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
