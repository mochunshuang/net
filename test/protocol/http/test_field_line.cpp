#include "../test_head.hpp"
#include <cassert>

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // field-line = field-name ":" OWS field-value OWS
    constexpr auto field_line_pass = make_pass_test<field_line>();
    constexpr auto field_line_fail = make_unpass_test<field_line>();

    {
        static_assert(make_rule_test<field_line>("Content-Type: text/html"_span).first);

        static_assert(make_pass_test<field_value>()("text/html"_span));
    }
    static_assert("Content-Type: text/html"_span[12] == ':');

    // 有效 field-line 测试
    static_assert(field_line_pass("Content-Type: text/html"_span)); // 标准格式
    static_assert(field_line_pass("Host:   example.com  "_span));   // 前后空白

    static_assert(field_line_pass("X-Empty:"_span)); // 空字段值
    {
        static_assert(make_rule_test<field_value>(""_span).first);
    }

    static_assert(field_line_pass("Name: value"_span));        // 基本格式
    static_assert(field_line_pass("Content-Length: 42"_span)); // 数字值
    static_assert(field_line_pass(
        "If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT"_span)); // 复杂值
    {
        {
            // FieldLineRuleImpl
            static_assert(make_pass_test<FieldLineRuleImpl>()(
                "If-Modified-Since: Sat, 29 Oct 1994 19:43:31 GMT"_span));
        }
        {
            // NOTE: 如果匹配规则，则是成功的
            static_assert(make_pass_test<mcs::abnf::http::If_Modified_Since>()(
                "Sat, 29 Oct 1994 19:43:31 GMT"_span));
            static_assert(make_pass_test<mcs::abnf::http::IMF_fixdate>()(
                "Sat, 29 Oct 1994 19:43:31 GMT"_span));
        }
        // NOTE: 但是失败
        // static_assert(
        //     make_pass_test<field_value>("Sat, 29 Oct 1994 19:43:31 GMT"_span).first);
        {
            // field-value = *field-content
            // static_assert(
            //     make_pass_test<mcs::abnf::http::field_content>()("Sat, 29 Oct 1994
            //     19:43:31 GMT"_span));

            // NOTE: 但是组合解析失败. 理论上是成功的才对。 原因是重叠无法区分：
            // NOTE: field_line 空格结尾 是错误的来源只能手写了。 你看下面就是正确的
            static_assert(make_pass_test<field_line>()("If-Modified-Since: Sat, "_span));
            static_assert(
                make_pass_test<field_line>()("If-Modified-Since: Sat, 29"_span));
            /**
            * field-content = field-vchar [ 1*( SP / HTAB / field-vchar )
                                        field-vchar ]
            *
            */
            static_assert(make_pass_test<mcs::abnf::http::field_vchar>()("S"_span));
            static_assert(make_pass_test<mcs::abnf::http::field_vchar>()("T"_span));
            static_assert(make_pass_test<mcs::abnf::http::field_vchar>()(","_span));
            static_assert(make_pass_test<mcs::abnf::http::field_vchar>()(";"_span));
            static_assert(make_pass_test<mcs::abnf::http::field_vchar>()("/"_span));
            static_assert(make_pass_test<mcs::abnf::SP>()(" "_span));
            static_assert(make_pass_test<mcs::abnf::HTAB>()("\t"_span));

            static_assert(make_pass_test<mcs::abnf::http::field_vchar>()("\""_span));
        }
    }

    // 空白处理测试 (OWS = 可选空白)
    static_assert(field_line_pass("X-Tab:\tvalue\t"_span));         // 制表符空白
    static_assert(field_line_pass("X-Space:   value   "_span));     // 多空格
    static_assert(field_line_pass("X-Mixed:\t \tvalue\t \t"_span)); // 混合空白

    // 字段名边界测试 (RFC 9110 §5.1)
    static_assert(field_line_pass("!#$%&'*+-.^_`|~: token-chars"_span)); // 有效token字符
    static_assert(field_line_pass("X-123: numbers"_span));               // 数字在名称中

    // 字段值边界测试 (RFC 9110 §5.5)
    static_assert(field_line_pass("X-Quoted: \"quoted value\""_span)); // 引号包裹
    static_assert(field_line_pass("X-Comma: value1, value2"_span));    // 逗号分隔值
    {
        // static_assert(make_pass_test<field_value>()("\"quoted value\""_span));
        // field_content
        static_assert(make_pass_test<mcs::abnf::http::field_content>()("\"\"a"_span));
    }
    static_assert(field_line_pass("X-UTF8: 你好世界"_span)); // UTF-8字符

    static_assert(field_line_pass("Invalid Name: value"_span));        // 名称含空格
    static_assert(field_line_pass("X-Control\u0001Char: value"_span)); // 控制符在名称中
    static_assert(field_line_pass(
        "X-TrailingColon:: value"_span)); // 额外冒号.//NOTE: 第二个冒号开始是field_value

    // 无效 field-line 测试
    static_assert(!field_line_fail("X-NoColon value"_span)); // 缺少冒号
    static_assert(
        field_line_fail("X-InvalidValue: val\u000Bue"_span));   // 垂直制表符在值中
    static_assert(field_line_fail("X-NoCRLF: value\r\n"_span)); // 存在CRLF
    static_assert(!field_line_fail(": value"_span));            // 缺少字段名

    return 0;
}
// NOLINTEND