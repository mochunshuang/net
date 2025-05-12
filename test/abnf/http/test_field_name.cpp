#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // field-name = token
    constexpr auto field_name = make_pass_test<mcs::abnf::http::field_name>();
    constexpr auto not_field_name = make_unpass_test<mcs::abnf::http::field_name>();

    // 有效field-name测试
    static_assert(field_name("Content-Type"_span));
    static_assert(field_name("X-Requested-With"_span));
    static_assert(field_name("User123"_span));
    static_assert(field_name("!#$%&'*+-.^_`|~"_span)); // 特殊符号组合
    static_assert(not field_name(""_span));            // 空名称

    // 无效field-name测试
    static_assert(not_field_name("Content Type"_span));  // 含空格
    static_assert(not not_field_name("(invalid)"_span)); // 非法符号

    static_assert(not not_field_name("中文"_span)); // 非ASCII字符
    static_assert(not not_field_name("测试"_span)); // 非ASCII字符

    return 0;
}
// NOLINTEND