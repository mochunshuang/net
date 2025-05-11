#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 有效测试用例
    {
        constexpr auto token_rule = make_pass_test<token>();
        // 单个合法字符
        static_assert(token_rule("!"_span));
        static_assert(token_rule("~"_span));
        static_assert(token_rule("a"_span));
        static_assert(token_rule("Z"_span));
        static_assert(token_rule("5"_span));

        // 混合字符
        static_assert(token_rule("Content+Type~v1"_span));
        static_assert(token_rule("custom_token-123.456"_span));
        static_assert(token_rule("!#$%&'*+-.^_`|~"_span));

        // 超长合法字符串（编译期生成）
        constexpr auto long_token =
            "!#$%&'*+-.^_`|~"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"_span;
        static_assert(token_rule(long_token));

        // 空输入
        static_assert(!token_rule(""_span));

        static_assert(token_rule("end_with_"_span)); // 假设 '_' 合法（实际合法）
    }

    // 无效测试用例
    {
        constexpr auto token_rule = make_unpass_test<token>();

        // 包含非法字符
        static_assert(token_rule("hello world"_span));     // 空格
        static_assert(token_rule("name@domain"_span));     // '@' 非法
        static_assert(!token_rule("(invalid)"_span));      // '(', ')' 非法
        static_assert(token_rule("contains\"quote"_span)); // DQUOTE 非法

        // 首字符非法
        static_assert(!token_rule(" no_leading_space"_span));
        static_assert(!token_rule("\"quoted"_span));

        // 中间字符非法
        static_assert(token_rule("valid?token"_span)); // '?' 非法
        static_assert(token_rule("a\\b"_span));        // 反斜杠非法

        // 末尾字符非法
        static_assert(token_rule("end_with\0"_span)); // 空字符非法
    }

    // 错误位置验证
    {

        constexpr auto s1 = "good?token"_span;
        constexpr auto ret1 = make_rule_test<token>(s1);
        static_assert(ret1.first.has_value() && ret1.second.cur_index == 4 &&
                      !ret1.second.done()); // '?' 在索引4

        constexpr auto s2 = " space_start"_span;
        constexpr auto ret2 = make_rule_test<token>(s2);
        static_assert(not ret2.first && ret2.second.cur_index == 0 &&
                      !ret1.second.done()); // 首字符空格

        constexpr auto s3 = "valid\0x01"_span;
        constexpr auto ret3 = make_rule_test<token>(s3);
        static_assert(ret3.first && ret3.second.cur_index == 5 &&
                      !ret1.second.done()); // 第5字节非法
    }
    return 0;
}
// NOLINTEND