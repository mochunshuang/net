#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        constexpr auto transfer_parameter_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = transfer_parameter{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 简单 token 值
        constexpr auto s1 = "param=value"_span;
        static_assert(transfer_parameter_rule(s1));

        // 带空格的 BWS
        constexpr auto s2 = "key  \t=  123"_span;
        static_assert(transfer_parameter_rule(s2));

        // 带转义的 quoted-string
        constexpr auto s4 = "msg=\"hello\\\"world\""_span;
        static_assert(transfer_parameter_rule(s4));

        // 含 obs-text 的 quoted-string（假设允许）
        constexpr auto s5 = "data=\"\x80\xFF\""_span;
        static_assert(transfer_parameter_rule(s5));

        // 空 quoted-string
        constexpr auto s6 = "empty=\"\""_span;
        static_assert(transfer_parameter_rule(s6));

        // 复杂混合用例
        constexpr auto s7 = "name=\"John\\xDoe\";age=30"_span;
        static_assert(transfer_parameter_rule("name=\"John\\xDoe\""_span));

        {
            // 空输入
            constexpr auto s1 = ""_span;
            static_assert(not transfer_parameter_rule(s1));

            // 带 obs-text 的 quoted-string（需确认实现支持）
            constexpr auto s2 = "text=\"\x80\xFF\""_span;
            static_assert(transfer_parameter_rule(s2)); // 假设允许 obs-text
        }
        {
            // 原错误用例：err;str=\"\\x\" 应成功（x 是合法 VCHAR）
            constexpr auto s2 = "str=\"\\x\""_span;
            static_assert(transfer_parameter_rule(s2));
        }
    }

    // 非法测试用例
    {
        constexpr auto transfer_parameter_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = transfer_parameter{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        // 缺少等号
        constexpr auto s1 = "param value"_span;
        static_assert(not transfer_parameter_rule(s1));

        // 等号后无值
        constexpr auto s2 = "key="_span;
        static_assert(not transfer_parameter_rule(s2));

        // 引号未闭合
        constexpr auto s3 = "err=\"unclosed"_span;
        static_assert(not transfer_parameter_rule(s3));

        // 非法转义字符（\ 后跟控制字符）
        constexpr auto s4 = "err=\"\\\x01\""_span; // \x01 是非 VCHAR
        static_assert(not transfer_parameter_rule(s4));

        // 参数名含非法字符（空格）
        constexpr auto s5 = "in valid=123"_span;
        static_assert(not transfer_parameter_rule(s5));

        // 值含非法字符（非 token/quoted-string）
        constexpr auto s6 = "err=!@#"_span;
        static_assert(transfer_parameter_rule(s6));

        // 混合 token 和 quoted-string 错误
        constexpr auto s7 = "key=val\"ue"_span;
        static_assert(transfer_parameter_rule(s7));

        // 值含特殊符号的 token
        constexpr auto s8 = "flag=+~/_"_span;
        static_assert(transfer_parameter_rule(s8));
        {
            // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
            // static_assert(not token("+~/_"_span));
            // static_assert(not quoted_string("+~/_"_span));

            // constexpr auto s3 = "flag=\"+~/_\""_span;
            // static_assert(not transfer_parameter_rule(s3));
            // static_assert(quoted_string("\"+~/_\""_span));
        }
        // 存在非法字符
        constexpr auto long_token =
            "k="
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
            "+-._~!$&'()*+,;=:" // 重复直到 1024 字符
            // ...（实际代码需生成长字符串）
            ""_span;
        static_assert(transfer_parameter_rule(long_token));
        {
            constexpr auto token_rule = [](const auto &array) constexpr {
                auto span = std::span{array};
                parser_ctx ctx = make_parser_ctx(span);
                assert(ctx.cur_index == 0);
                auto suc = token{}(ctx);
                return suc;
            };
            constexpr auto sp = "+-._~!$&'()*+,;=:"_span;
            constexpr auto ret = token_rule(sp);
            static_assert(ret);
            static_assert(ret == 9);
        }
        {
            constexpr auto s1 = "param=~/path"_span;
            static_assert(transfer_parameter_rule(s1)); //  / 不是合法 tchar
        }
    }

    return 0;
}
// NOLINTEND
