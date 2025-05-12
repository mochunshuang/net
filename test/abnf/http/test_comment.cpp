#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // comment = "(" *( ctext / quoted-pair / comment ) ")"
    // quoted-pair = "\" ( HTAB / SP / VCHAR / obs-text )
    constexpr auto comment_pass = make_pass_test<mcs::abnf::http::comment>();
    constexpr auto comment_fail = make_unpass_test<mcs::abnf::http::comment>();

    // 有效comment测试
    static_assert(comment_pass("(simple comment)"_span));   // 基础注释
    static_assert(comment_pass("((nested comment))"_span)); // 嵌套注释
    static_assert(comment_pass("(escaped \\))"_span));      // 转义处理
    static_assert(comment_pass("(bad %)"_span));            // 非法符号
    static_assert(comment_pass("()"_span));                 // 空注释

    static_assert(comment_pass("(escaped \"adad\\r\\n\\t\" )"_span)); // 转义处理

    // 测试嵌套注释（RFC 7230§3.2.6）
    static_assert(comment_pass("(test (nested) comment)"_span)); // 嵌套注释

    // 无效comment测试
    static_assert(not comment_fail("(multi\nline\ttext)"_span)); // 特殊字符
    static_assert(not comment_fail("unclosed (comment"_span));   // 括号未闭合
    static_assert(not comment_fail("(invalid\x01char)"_span));   // 控制字符

    return 0;
}
// NOLINTEND