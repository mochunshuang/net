#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{

    constexpr auto http_name_pass = make_pass_test<HTTP_name>();
    constexpr auto http_name_fail = make_unpass_test<HTTP_name>();

    // 有效HTTP-name测试
    static_assert(http_name_pass("HTTP"_span)); // 完全匹配

    static_assert(http_name_fail("HTTP123"_span)); // 能匹配4字节,当时有剩余字符

    // 无效HTTP-name测试
    static_assert(not http_name_fail("HTT"_span));  // 过短
    static_assert(not http_name_fail("HTPP"_span)); // 字符顺序错误
    static_assert(not http_name_fail("http"_span)); // 大小写不匹配
    static_assert(http_name_fail("HTTP/"_span));    /// 能匹配4字节,当时有剩余字符

    return 0;
}
// NOLINTEND