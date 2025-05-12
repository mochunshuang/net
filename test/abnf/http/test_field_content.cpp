#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // field-content = field-vchar [ 1*( SP / HTAB / field-vchar ) ]
    // field-vchar = VCHAR / obs-text
    //  VCHAR          =  %x21-7E; visible (printing) characters
    //  obs-text = %x80-FF
    constexpr auto field_content = make_pass_test<mcs::abnf::http::field_content>();
    constexpr auto not_field_content = make_unpass_test<mcs::abnf::http::field_content>();

    // 有效field-content测试
    static_assert(field_content("a"_span));

    // 无效field-content测试
    static_assert(not_field_content("abc"_span));
    {
        //  VCHAR          =  %x21-7E; visible (printing) characters
        static_assert(make_unpass_test<mcs::abnf::VCHAR>()("abc"_span));
    }
    static_assert(not_field_content("a b"_span));     // 含空格
    static_assert(not_field_content("a\tb"_span));    // 含制表符
    static_assert(not_field_content("a!@#$%"_span));  // 特殊字符
    static_assert(not_field_content("a b_c-d"_span)); // 混合内容

    static_assert(not field_content(""_span));         // 空内容
    static_assert(not not_field_content(" abc"_span)); // 起始空格
    static_assert(not_field_content("a\nb"_span));     // 含换行符
    static_assert(not_field_content("a\001b"_span));   // 控制字符
    static_assert(not_field_content("测试"_span));     // 非ASCII字符

    return 0;
}
// NOLINTEND