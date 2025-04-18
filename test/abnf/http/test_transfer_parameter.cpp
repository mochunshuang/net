#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 简单 token 值
        constexpr auto s1 = "param=value"_span;
        static_assert(transfer_parameter(s1));

        // 带空格的 BWS
        constexpr auto s2 = "key  \t=  123"_span;
        static_assert(transfer_parameter(s2));

        // 值含特殊符号的 token
        constexpr auto s3 = "flag=+~/_"_span;
        static_assert(not transfer_parameter(s3));
        {
            // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
            static_assert(not token("+~/_"_span));
            static_assert(not quoted_string("+~/_"_span));

            constexpr auto s3 = "flag=\"+~/_\""_span;
            static_assert(transfer_parameter(s3));
            static_assert(quoted_string("\"+~/_\""_span));
        }

        // 带转义的 quoted-string
        constexpr auto s4 = "msg=\"hello\\\"world\""_span;
        static_assert(transfer_parameter(s4));

        // 含 obs-text 的 quoted-string（假设允许）
        constexpr auto s5 = "data=\"\x80\xFF\""_span;
        static_assert(transfer_parameter(s5));

        // 空 quoted-string
        constexpr auto s6 = "empty=\"\""_span;
        static_assert(transfer_parameter(s6));

        // 复杂混合用例
        constexpr auto s7 = "name=\"John\\xDoe\";age=30"_span;
        static_assert(transfer_parameter("name=\"John\\xDoe\""_span));
    }

    // 非法测试用例
    {
        // 缺少等号
        constexpr auto s1 = "param value"_span;
        static_assert(not transfer_parameter(s1));

        // 等号后无值
        constexpr auto s2 = "key="_span;
        static_assert(not transfer_parameter(s2));

        // 引号未闭合
        constexpr auto s3 = "err=\"unclosed"_span;
        static_assert(not transfer_parameter(s3));

        // 非法转义字符（\ 后跟控制字符）
        constexpr auto s4 = "err=\"\\\x01\""_span; // \x01 是非 VCHAR
        static_assert(not transfer_parameter(s4));

        // 参数名含非法字符（空格）
        constexpr auto s5 = "in valid=123"_span;
        static_assert(not transfer_parameter(s5));

        // 值含非法字符（非 token/quoted-string）
        constexpr auto s6 = "err=!@#"_span;
        static_assert(not transfer_parameter(s6));

        // 混合 token 和 quoted-string 错误
        constexpr auto s7 = "key=val\"ue"_span;
        static_assert(not transfer_parameter(s7));
    }

    // 边界测试
    {
        // 空输入
        constexpr auto s1 = ""_span;
        static_assert(not transfer_parameter(s1));

        // 极长合法参数（token 长度 1024）
        constexpr auto long_token =
            "k="
            "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
            "+-._~!$&'()*+,;=:" // 重复直到 1024 字符
            // ...（实际代码需生成长字符串）
            ""_span;
        static_assert(not transfer_parameter(long_token));
        {
            constexpr auto sp = "+-._~!$&'()*+,;=:"_span;
            constexpr auto ret = token(sp);
            static_assert(not ret);
            static_assert(ret.error().index() == 9);
            static_assert(sp[ret.error().index()] == '(');
            static_assert(not tchar(sp[9]));
        }

        // 带 obs-text 的 quoted-string（需确认实现支持）
        constexpr auto s2 = "text=\"\x80\xFF\""_span;
        static_assert(transfer_parameter(s2)); // 假设允许 obs-text
    }

    {
        constexpr auto s1 = "param=~/path"_span;
        static_assert(not transfer_parameter(s1)); //  / 不是合法 tchar
        {
            static_assert(not token("~/path"_span));
            static_assert(tchar('~'));

            static_assert(not tchar('/'));
            static_assert(not token("/"_span));
        }

        // 原错误用例：err;str=\"\\x\" 应成功（x 是合法 VCHAR）
        constexpr auto s2 = "str=\"\\x\""_span;
        static_assert(transfer_parameter(s2));
    }

    return 0;
}
// NOLINTEND
