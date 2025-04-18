#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 有效测试用例
    {
        // 单个合法字符
        static_assert(token("!"_span));
        static_assert(token("~"_span));
        static_assert(token("a"_span));
        static_assert(token("Z"_span));
        static_assert(token("5"_span));

        // 混合字符
        static_assert(token("Content+Type~v1"_span));
        static_assert(token("custom_token-123.456"_span));
        static_assert(token("!#$%&'*+-.^_`|~"_span));

        // 超长合法字符串（编译期生成）
        constexpr auto long_token =
            "!#$%&'*+-.^_`|~"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_span;
        static_assert(token(long_token));
    }

    // 无效测试用例
    {
        // 空输入
        static_assert(!token(""_span));

        // 包含非法字符
        static_assert(!token("hello world"_span));     // 空格
        static_assert(!token("name@domain"_span));     // '@' 非法
        static_assert(!token("(invalid)"_span));       // '(', ')' 非法
        static_assert(!token("contains\"quote"_span)); // DQUOTE 非法

        // 首字符非法
        static_assert(!token(" no_leading_space"_span));
        static_assert(!token("\"quoted"_span));

        // 中间字符非法
        static_assert(!token("valid?token"_span)); // '?' 非法
        static_assert(!token("a\\b"_span));        // 反斜杠非法

        // 末尾字符非法
        static_assert(token("end_with_"_span));   // 假设 '_' 合法（实际合法）
        static_assert(!token("end_with\0"_span)); // 空字符非法
    }

    // 错误位置验证
    {
        constexpr auto s1 = "good?token"_span;
        constexpr auto ret1 = token(s1);
        static_assert(not ret1 && ret1.error().index() == 4); // '?' 在索引4

        constexpr auto s2 = " space_start"_span;
        constexpr auto ret2 = token(s2);
        static_assert(not ret2 && ret2.error().index() == 0); // 首字符空格

        constexpr auto s3 = "valid\0x01"_span;
        constexpr auto ret3 = token(s3);
        static_assert(not ret3 && ret3.error().index() == 5); // 第5字节非法
    }
    return 0;
}
// NOLINTEND