#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) field-vchar ]
    // field-vchar = VCHAR / obs-text
    //  VCHAR          =  %x21-7E; visible (printing) characters
    //  obs-text = %x80-FF
    constexpr auto field_content = make_pass_test<mcs::abnf::http::field_content>();
    constexpr auto not_field_content = make_unpass_test<mcs::abnf::http::field_content>();

    // 有效field-content测试
    static_assert(field_content("a"_span));

    // NOTE: 全部是 field-vchar 也是合理的， 只要开头和尾部是 field-vchar 字符即可
    static_assert(field_content("abc"_span));
    {
        //  VCHAR          =  %x21-7E; visible (printing) characters
        static_assert(make_unpass_test<mcs::abnf::VCHAR>()("abc"_span));
    }
    {
        // NOTE: SP 不就是空格吗？
        // SP             =  %x20
        static_assert(!make_pass_test<mcs::abnf::SP>()(""_span));
        static_assert(make_pass_test<mcs::abnf::SP>()(" "_span));
        static_assert(' ' == 0x20);

        // TODO:应该设计一个  then 算法 而不是
        // TODO: 应该设计一个then 算法
        static_assert(make_pass_test<field_vchar>()("a"_span));
        // TODO: BUG。
        // NOTE:sequence应该是一个状态机。第一个规则完毕才进行下一个测试，不能一个错误就全部错误
        //  static_assert(field_content("a "_span));
    }

    static_assert(field_content("\""_span));    // 双引号
    static_assert(field_content("\" \""_span)); // 双引号
    static_assert(field_content("a b"_span));   // 含空格
    static_assert(field_content("a\tb"_span));  // 含制表符

    static_assert(field_content("a!@#$%"_span)); // 特殊字符
    {
        static_assert(make_pass_test<field_vchar>()("%"_span));
        static_assert(make_pass_test<field_vchar>()("$"_span));
        static_assert(make_pass_test<field_vchar>()("#"_span));
        static_assert(make_pass_test<field_vchar>()("@"_span));
        static_assert(make_pass_test<field_vchar>()("!"_span));

        // NOTE: \n\t 不属于
        {
            constexpr auto span = "\n"_span;
            static_assert(span.size() == 1);
            static_assert(!make_rule_test<mcs::abnf::VCHAR>(span).first);
        }
        {
            constexpr auto span = "\t"_span;
            static_assert(span.size() == 1);
            static_assert(!make_rule_test<mcs::abnf::VCHAR>(span).first);
        }
        {
            constexpr auto span = "\n"_span;
            static_assert(span.size() == 1);
            static_assert(!make_rule_test<obs_text>(span).first);
        }
        {
            constexpr auto span = "\t"_span;
            static_assert(span.size() == 1);
            static_assert(!make_rule_test<obs_text>(span).first);
        }
    }

    static_assert(field_content("a b_c-d"_span)); // 混合内容

    static_assert(!field_content(""_span)); // 空内容

    static_assert(
        field_content("测试"_span)); // NOTE: 非ASCII字符，数组符合即可。不在乎如何解释

    static_assert(not_field_content("ab"_span));     // NOTE: 2 个字符是不行的
    static_assert(!not_field_content(" abc"_span));  // 起始空格
    static_assert(not_field_content("a\nb"_span));   // 含换行符
    static_assert(not_field_content("a\001b"_span)); // 控制字符

    return 0;
}
// NOLINTEND