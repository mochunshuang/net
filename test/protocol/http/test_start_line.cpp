#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // start-line = request-line / status-line
    // ==================== start-line 综合测试 ====================
    constexpr auto start_line_pass = make_pass_test<start_line>();
    constexpr auto start_line_fail = make_unpass_test<start_line>();

    // 有效 start-line 测试 (request-line 和 status-line)
    static_assert(start_line_pass("GET / HTTP/1.1"_span));        // request-line
    static_assert(start_line_pass("HTTP/1.1 200 OK"_span));       // status-line
    static_assert(start_line_pass("CUSTOM /path HTTP/1.1"_span)); // 自定义方法
    static_assert(start_line_pass("HTTP/1.1 404 "_span));         // 空原因短语

    // 无效 start-line 测试
    static_assert(!start_line_fail("INVALID LINE"_span));                 // 无法识别类型
    static_assert(start_line_fail("GET / HTTP/1.1\r\n"_span));            // 无效行尾(LF)
    static_assert(start_line_fail("HTTP/1.1 200 OK\n"_span));             // 无效行尾(LF)
    static_assert(start_line_fail("GET / HTTP/1.1X-Header: value"_span)); // 多行内容
    static_assert(start_line_fail("HTTP/1.1 200 OK\r\nX-Header: value"_span)); // 多行内容

    return 0;
}
// NOLINTEND