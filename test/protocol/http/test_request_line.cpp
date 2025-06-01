#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // ==================== request-line 规则测试 ====================
    constexpr auto request_line_pass = make_pass_test<request_line>();
    constexpr auto request_line_fail = make_unpass_test<request_line>();

    // 有效 request-line 测试
    static_assert(request_line_pass("GET / HTTP/1.1"_span));               // 最小有效格式
    static_assert(request_line_pass("POST /submit?q=term HTTP/1.0"_span)); // 带查询参数
    static_assert(
        request_line_pass("CONNECT example.com:443 HTTP/1.1"_span)); // authority-form
    static_assert(request_line_pass("OPTIONS * HTTP/1.1"_span));     // asterisk-form
    static_assert(
        request_line_pass("GET /path%20with%20spaces HTTP/1.1"_span)); // 百分比编码
    static_assert(request_line_pass("CUSTOM-METHOD / HTTP/1.1"_span)); // 自定义方法

    // 无效 request-line 测试
    static_assert(!request_line_fail("GET  /  HTTP/1.1"_span));    // 多余空格
    static_assert(!request_line_fail("GET/ HTTP/1.1"_span));       // 缺少方法后空格
    static_assert(!request_line_fail("GET /HTTP/1.1"_span));       // 缺少目标后空格
    static_assert(!request_line_fail("GÉT / HTTP/1.1"_span));      // 非法方法字符
    static_assert(!request_line_fail("GET / \tHTTP/1.1"_span));    // 制表符代替空格
    static_assert(request_line_fail("GET / HTTP/1.1 extra"_span)); // 额外内容
    static_assert(request_line_fail("GET / HTTP/1.1X-Header: value"_span)); // 多行内容

    return 0;
}
// NOLINTEND