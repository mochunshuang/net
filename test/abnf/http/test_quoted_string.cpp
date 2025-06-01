#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // quoted-string = DQUOTE *( qdtext / quoted-pair ) DQUOTE
    // ==================== quoted-string 规则测试 ====================
    constexpr auto quoted_string_pass = make_pass_test<quoted_string>();
    constexpr auto quoted_string_fail = make_unpass_test<quoted_string>();

    // 有效 quoted-string 测试
    static_assert(quoted_string_pass("\"\""_span));                    // 空引号
    static_assert(quoted_string_pass("\"abc\""_span));                 // 基础字符串
    static_assert(quoted_string_pass("\" \\\" \""_span));              // 转义双引号
    static_assert(quoted_string_pass("\"line\\r\\n\""_span));          // 转义回车换行
    static_assert(quoted_string_pass("\" \\\\ \""_span));              // 转义反斜杠
    static_assert(quoted_string_pass("\"hello \\\"world\\\"\""_span)); // 混合内容
    static_assert(quoted_string_pass("\" space \t inside \""_span));   // 空白字符
    static_assert(quoted_string_pass("\"!@#$%^&*()\""_span));          // 特殊字符
    static_assert(quoted_string_pass("\"\\x7F\""_span));               // 转义控制字符
    static_assert(quoted_string_pass("\"\\u1234\""_span));             // 转义Unicode

    static_assert(quoted_string_pass("\"bad escape \\ \""_span)); // 无效转义序列

    static_assert(quoted_string_pass("\" \\r \""_span)); // 转义序列
    static_assert(quoted_string_pass("\"\\r\""_span));
    static_assert(quoted_string_pass(
        "\"\\\"\""_span)); // NOTE: 中间单引号。\\ 才是一个 "\"。而\"才是"""
    static_assert("\"\\\"\""_span.size() == 4);

    // 无效 quoted-string 测试
    static_assert(!quoted_string_fail("\""_span));                  // 未闭合引号
    static_assert(!quoted_string_fail("abc\""_span));               // 缺少起始引号
    static_assert(!quoted_string_fail("\"abc"_span));               // 缺少结束引号
    static_assert(!quoted_string_fail("\"unterminated\\\""_span));  // 未终止的转义
    static_assert(!quoted_string_fail("\"invalid\x01char\""_span)); // 非法控制字符

    static_assert(quoted_string_fail("\"unescaped\"quote\""_span)); // 未转义引号
    static_assert(!quoted_string_fail("no_quotes"_span));           // 完全无引号
    static_assert(!quoted_string_fail("\"\\\""_span));              // 转义后无字符

    return 0;
}
// NOLINTEND