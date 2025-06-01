#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{

    constexpr auto http_version_pass = make_pass_test<HTTP_version>();
    constexpr auto http_version_fail = make_unpass_test<HTTP_version>();

    // 有效HTTP-version测试
    static_assert(http_version_pass("HTTP/1.1"_span)); // 标准格式
    static_assert(http_version_pass("HTTP/1.0"_span)); // 有效版本
    static_assert(http_version_pass("HTTP/9.9"_span)); // 最大版本号
    static_assert(http_version_pass("HTTP/0.0"_span)); // 最小版本号

    static_assert(http_version_fail("HTTP/1.15"_span));    // 次版本号>9
    static_assert(http_version_fail("HTTP/1.1\r\n"_span)); // 有剩余字符未能完全匹配

    // 无效HTTP-version测试
    static_assert(not http_version_fail("HTTP1.1"_span));   // 缺少斜杠
    static_assert(not http_version_fail("HTTP/1."_span));   // 次版本号缺失
    static_assert(not http_version_fail("HTTP/1_1"_span));  // 使用下划线替代点
    static_assert(not http_version_fail("http/1.1"_span));  // 小写http
    static_assert(not http_version_fail("HTTP/01.1"_span)); // 主版本号前导零

    static_assert(http_version_fail("HTTP/1.01"_span)); // 有剩余字符未能完全匹配
    static_assert(http_version_fail("HTTP/1.1 "_span)); // 有剩余字符未能完全匹配
    static_assert(http_version_fail("HTTP/1.1x"_span)); // 有剩余字符未能完全匹配

    return 0;
}
// NOLINTEND