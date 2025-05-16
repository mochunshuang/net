#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{

    constexpr auto http_message_pass = make_pass_test<HTTP_message>();
    constexpr auto http_message_fail = make_unpass_test<HTTP_message>();

    // 有效HTTP消息测试
    static_assert(http_message_pass("GET / HTTP/1.1\r\n\r\n"_span)); // 无头部和body
    static_assert(http_message_pass(
        "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"_span)); // 包含头部
    static_assert(http_message_pass(
        "POST /submit HTTP/1.1\r\nContent-Length: 5\r\n\r\nhello"_span)); // 含消息体
    static_assert(http_message_pass(
        "GET / HTTP/1.1\r\nHost: example.com\r\nAccept: */*\r\n\r\n"_span)); // 多头部字段
    static_assert(
        http_message_pass("HTTP/1.1 204 No Content\r\n\r\n"_span)); // 无body的响应

    // 无效HTTP消息测试
    static_assert(not http_message_fail("GET / HTTP/1.1\r\n"_span));     // 缺少结尾CRLF
    static_assert(not http_message_fail("GET / HTTP/1.1\n\n\r\n"_span)); // 使用LF而非CRLF
    static_assert(not http_message_fail(
        "GET / HTTP/1.1\r\nHost example.com\r\n\r\n"_span)); // 头部字段格式错误（缺少冒号）
    static_assert(not http_message_fail(
        "GET / HTTP/1.1\r\nHost: example.com\r\nContent-Length: 5\r\n"_span)); // 结尾CRLF缺失
    static_assert(
        not http_message_fail("INVALIDSTARTLINE\r\n\r\n"_span)); // 非法start-line格式

    return 0;
}
// NOLINTEND