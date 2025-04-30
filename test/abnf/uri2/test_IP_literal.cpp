
#include "../test_abnf.hpp"
#include "./test_uri.hpp"
#include <cassert>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;

#include <iostream>

int main()
{

    // 有效测试用例
    {
        constexpr auto ipliteral_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::IP_literal{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        // IPv6 有效用例
        constexpr auto valid_ipv6_1 = "[2001:db8::1]"_ctx;         // 标准 IPv6 缩写
        constexpr auto valid_ipv6_2 = "[::1]"_ctx;                 // 最小化 IPv6 地址
        constexpr auto valid_ipv6_3 = "[1:2:3:4:5:6:7:8]"_ctx;     // 全展开无缩写
        constexpr auto valid_ipv6_4 = "[::ffff:192.168.0.1]"_ctx;  // IPv4 嵌入 ls32
        constexpr auto valid_ipv6_5 = "[A:bCd:0FF:0:0:0:0:0]"_ctx; // 混合大小写 HEXDIG
        constexpr auto valid_ipv6_6 = "[a::b:1.2.3.4]"_ctx;        // 合法 ls32 格式

        // IPvFuture 有效用例
        constexpr auto valid_future_1 = "[v1.a]"_ctx;       // 最小合法结构
        constexpr auto valid_future_2 = "[vF.~]"_ctx;       // unreserved 字符
        constexpr auto valid_future_3 = "[v123456.:+]"_ctx; // 长 HEXDIG + 混合合法字符
        constexpr auto valid_future_4 = "[vA.!$&'()*+,;=]"_ctx; // 全 sub-delims
        constexpr auto valid_future_5 = "[v7._:a~]"_ctx;        // 混合 unreserved 和冒号

        constexpr auto valid_mix_1 = "[v1.::1]"_ctx; // 混合 IPvFuture 和 IPv6

        // 有效用例断言
        static_assert(ipliteral_rule(valid_ipv6_1));
        static_assert(ipliteral_rule(valid_ipv6_2));
        static_assert(ipliteral_rule(valid_ipv6_3));
        static_assert(ipliteral_rule(valid_ipv6_4));

        static_assert(ipliteral_rule(valid_ipv6_5));
        static_assert(ipliteral_rule(valid_ipv6_6));
        static_assert(ipliteral_rule(valid_future_1));
        static_assert(ipliteral_rule(valid_future_2));
        static_assert(ipliteral_rule(valid_future_3));
        static_assert(ipliteral_rule(valid_future_4));
        static_assert(ipliteral_rule(valid_future_5));

        static_assert(ipliteral_rule(valid_mix_1));
    }
    {
        constexpr auto ipliteral_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::IP_literal{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        // 格式错误（不匹配方括号）
        constexpr auto invalid_fmt_1 = "2001:db8::1"_ctx;  // 缺少方括号
        constexpr auto invalid_fmt_2 = "[2001:db8::1"_ctx; // 缺少右括号
        constexpr auto invalid_fmt_3 = "2001:db8::1]"_ctx; // 缺少左括号
        constexpr auto invalid_fmt_4 = "[]"_ctx;           // 空内容

        // IPv6 无效用例
        constexpr auto invalid_ipv6_1 = "[:::1]"_ctx;              // 连续三个冒号
        constexpr auto invalid_ipv6_2 = "[1:2:3:4:5:6:7:8:9]"_ctx; // 超过 8 个 h16
        constexpr auto invalid_ipv6_3 = "[::g]"_ctx;               // 非法 HEXDIG 'g'
        constexpr auto invalid_ipv6_4 = "[1::2::3]"_ctx;           // 多个双冒号缩写
        constexpr auto invalid_ipv6_5 = "[::1.2.3.256]"_ctx;       // 非法 IPv4 地址

        // IPvFuture 无效用例
        constexpr auto invalid_future_1 = "[vg.x]"_ctx;      // 'g' 非 HEXDIG
        constexpr auto invalid_future_2 = "[v1.]"_ctx;       // 尾部为空
        constexpr auto invalid_future_3 = "[v1.#]"_ctx;      // gen-delims '#'
        constexpr auto invalid_future_4 = "[v1. /]"_ctx;     // 包含 '/' 空格
        constexpr auto invalid_future_5 = "[v123.abc%]"_ctx; // 非法字符 '%'

        constexpr auto invalid_mix_2 = "[2001:db8:v1.a]"_ctx; // IPv6 中出现 'v'

        static_assert(!ipliteral_rule(invalid_fmt_1));
        static_assert(!ipliteral_rule(invalid_fmt_2));
        static_assert(!ipliteral_rule(invalid_fmt_3));
        static_assert(!ipliteral_rule(invalid_fmt_4));
        static_assert(!ipliteral_rule(invalid_ipv6_1));
        static_assert(!ipliteral_rule(invalid_ipv6_2));
        static_assert(!ipliteral_rule(invalid_ipv6_3));
        static_assert(!ipliteral_rule(invalid_ipv6_4));
        static_assert(!ipliteral_rule(invalid_ipv6_5));
        static_assert(!ipliteral_rule(invalid_future_1));
        static_assert(!ipliteral_rule(invalid_future_2));
        static_assert(!ipliteral_rule(invalid_future_3));
        static_assert(!ipliteral_rule(invalid_future_4));
        static_assert(!ipliteral_rule(invalid_future_5));

        static_assert(!ipliteral_rule(invalid_mix_2));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
