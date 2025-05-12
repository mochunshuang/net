#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Content-Type = media-type
    constexpr auto content_type_pass = make_pass_test<mcs::abnf::http::Content_Type>();
    constexpr auto content_type_fail = make_unpass_test<mcs::abnf::http::Content_Type>();
    static_assert(content_type_pass("text/html; charset=UTF-8"_span)); // 带参数
    static_assert(content_type_pass("application/json"_span));         // 基础类型

    static_assert(not content_type_fail("text/"_span)); // 不完整类型

    // 测试带注释的媒体类型参数
    static_assert(content_type_fail("text/html; charset=(utf-8)"_span));
    return 0;
}
// NOLINTEND