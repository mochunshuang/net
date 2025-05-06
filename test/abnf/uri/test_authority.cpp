
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
        constexpr auto authority_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = authority{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 完整结构：userinfo + IPv6 + port
        constexpr auto full_authority_1 =
            "user%3Apass@[v9.fe]:8080"_span; // 混合编码和IPvFuture
        static_assert(authority_rule(full_authority_1));

        // 仅有 userinfo 和 reg-name
        constexpr auto user_host_case = "~john:test@example%2Ecom"_span; // 带编码点的域名
        static_assert(authority_rule(user_host_case));

        // 仅有 IPv4 和 port; 没对 port 进行限制 65536 也是对的
        constexpr auto ipv4_port = "192.168.0.1:65535"_span; // 最大端口号
        static_assert(authority_rule(ipv4_port));
        constexpr auto ipv4_port2 = "192.168.0.1:65536"_span;
        static_assert(authority_rule(ipv4_port2));

        // 纯 reg-name 带特殊字符
        constexpr auto reg_name_special =
            "server_!$&.sub-domain"_span; // 包含sub-delims的注册名
        static_assert(authority_rule(reg_name_special));

        // 边界用例：空port（仅冒号）
        constexpr auto empty_port = "[::1]:"_span; // 允许port为空字符串
        static_assert(authority_rule(empty_port));
    }

    // 无效测试用例
    {
        constexpr auto authority_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = authority{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };
        // 结构错误：多重@符号
        constexpr auto multi_at = "user@name@[::1]"_span;
        static_assert(authority_rule(multi_at).value() == 9);

        // IPv6未加方括号
        constexpr auto unbraced_ipv6 = "2001:db8::1"_span;
        static_assert(authority_rule(unbraced_ipv6));

        // 非法port字符
        constexpr auto non_digit_port = "localhost:80a"_span; // 字母结尾
        static_assert(authority_rule(non_digit_port));

        // userinfo包含非法字符（空格）
        constexpr auto space_in_user = "user name@host"_span;
        static_assert(authority_rule(space_in_user));

        // reg-name包含保留字符（未编码的#）
        constexpr auto invalid_reg_name = "bad#host.com"_span;
        static_assert(authority_rule(invalid_reg_name));

        // 混合错误：错误IPv4格式+非法userinfo
        constexpr auto multi_error = "user@[::1:8080"_span; // IPv6缺少闭合括号
        static_assert(authority_rule(multi_error));
    }

    // build 测试
    {
        constexpr auto authority_rule = authority{};
        // 完整结构：userinfo + IPv6 + port
        {
            constexpr auto full_authority_1 =
                "user%3Apass@[v9.fe]:8080"_span; // 混合编码和IPvFuture
            auto ctx = make_parser_ctx(full_authority_1);
            auto ret = authority_rule.parse(ctx);
            assert(ret);
            assert(authority::result_type::userinfo_t::domain::buildString(
                       (*ret).userinfo.value()) == std::string("user%3Apass"));
            assert(authority::result_type::host_t::domain::buildString((*ret).host) ==
                   std::string("[v9.fe]"));
            std::cout << "ctx.cur_index: " << ctx.cur_index << '\n';
            assert(ctx.cur_index == full_authority_1.size());
            assert((*ret).port.has_value());
            assert(authority::result_type::port_t::domain::buildString(
                       (*ret).port.value()) == std::string("8080"));
            {
                auto rule =
                    make_optional{sequence{CharRule<CharSensitive<':'>>{}, port{}}};
                constexpr auto full_authority_1 = ":8080"_span;
                auto ctx = make_parser_ctx(full_authority_1);
                rule.parse(ctx);
                assert(ctx.cur_index == full_authority_1.size());
            }
            assert((*ret).userinfo.has_value());
            assert((*ret).port.has_value());
            std::cout << authority::buildString(*ret) << '\n';
            //
            assert(authority::buildString(*ret) ==
                   std::string("user%3Apass@[v9.fe]:8080"));
        }
        {
            // 仅有 userinfo 和 reg-name
            constexpr auto user_host_case =
                "~john:test@example%2Ecom"_span; // 带编码点的域名
            auto ctx = make_parser_ctx(user_host_case);
            auto ret = authority_rule.parse(ctx);
            assert(authority::buildString(*ret) ==
                   std::string("~john:test@example%2Ecom"));
        }
        {
            // 仅有 IPv4 和 port; 没对 port 进行限制 65536 也是对的
            constexpr auto ipv4_port = "192.168.0.1:65535"_span;
            auto ctx = make_parser_ctx(ipv4_port);
            auto ret = authority_rule.parse(ctx);
            assert(authority::buildString(*ret) == std::string("192.168.0.1:65535"));
        }
        {
            // 边界用例：空port（仅冒号）
            constexpr auto empty_port = "[::1]:"_span; // 允许port为空字符串
            auto ctx = make_parser_ctx(empty_port);
            auto ret = authority_rule.parse(ctx);
            assert(authority::buildString(*ret) == std::string("[::1]:"));
        }
        {
            // NOTE: 需要一个包装函数，来统一处理和判断是否真的解析完毕
            //  无效测试用例
            //  结构错误：多重@符号
            constexpr auto multi_at = "user@name@[::1]"_span;
            auto ctx = make_parser_ctx(multi_at);
            auto ret = authority_rule.parse(ctx);
            assert(authority::buildString(*ret) == std::string("user@name"));

            assert(not ctx.done());
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
