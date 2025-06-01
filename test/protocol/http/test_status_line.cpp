#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // status-line = HTTP-version SP status-code SP [ reason-phrase ]
    // ==================== status-line 规则测试 ====================
    constexpr auto status_line_pass = make_pass_test<status_line>();
    constexpr auto status_line_fail = make_unpass_test<status_line>();

    // 有效 status-line 测试
    static_assert(status_line_pass("HTTP/1.1 200 OK"_span)); // 带原因短语
    static_assert(status_line_pass("HTTP/1.0 404 "_span));   // 空原因短语
    static_assert(
        status_line_pass("HTTP/1.1 301 Moved Permanently"_span));    // 多词原因短语
    static_assert(status_line_pass("HTTP/1.1 100 \tContinue"_span)); // 制表符在原因短语中
    static_assert(status_line_pass("HTTP/1.1 200 \x21-\x7E"_span));  // 可见字符范围
    static_assert(status_line_pass("HTTP/1.1 200 \x80\xFF"_span)); // obs-text (扩展字符)

    // 无效 status-line 测试
    static_assert(!status_line_fail("HTTP/1.1 200OK"_span));           // 状态码后缺少空格
    static_assert(!status_line_fail("HTTP/1.1  200 OK"_span));         // 多余空格
    static_assert(status_line_fail("HTTP/1.1 200 \x01"_span));         // 控制字符(U+0001)
    static_assert(!status_line_fail("HTTP/1.1 20A Bad Request"_span)); // 非数字状态码
    static_assert(!status_line_fail("http/1.1 200 OK"_span));          // 小写HTTP
    static_assert(status_line_fail("HTTP/1.1 200 OK \r \n"_span));     // 无效行尾(LFCR)

    return 0;
}
// NOLINTEND