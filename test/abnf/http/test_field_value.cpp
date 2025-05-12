#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // field-value = *field-content
    constexpr auto field_value = make_pass_test<mcs::abnf::http::field_value>();
    constexpr auto not_field_value = make_unpass_test<mcs::abnf::http::field_value>();

    // 有效field-content测试
    static_assert(field_value("a"_span));
    static_assert(field_value("abc"_span));
    static_assert(field_value("a!@#$%"_span)); // 特殊字符
    static_assert(field_value("测试"_span));   // 非ASCII字符
    {
        static_assert("测试"_span.size() == 6);
    }
    static_assert(not field_value(""_span)); // 允许空值
    static_assert(field_value("value"_span));

    // 无效field-content测试
    static_assert(not_field_value("a b"_span));  // 含空格
    static_assert(not_field_value("a\tb"_span)); // 含制表符

    static_assert(not_field_value("a b_c-d"_span)); // 混合内容

    static_assert(not field_value(""_span));         // 空内容
    static_assert(not not_field_value(" abc"_span)); // 起始空格
    static_assert(not_field_value("a\nb"_span));     // 含换行符
    static_assert(not_field_value("a\001b"_span));   // 控制字符

    static_assert(not_field_value("value with spaces"_span));
    static_assert(not_field_value("a\tb%20c"_span)); // 含编码字符

    static_assert(not not_field_value("\x01\x02"_span)); // 控制字符
    static_assert(not_field_value("line\nbreak"_span));  // 换行符

    return 0;
}
// NOLINTEND