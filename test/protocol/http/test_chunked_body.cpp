#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // chunk-data = 1*OCTET
    // chunked-body = *chunk last-chunk trailer-section CRLF
    // chunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF
    // last-chunk = 1*"0" [ chunk-ext ] CRLF
    // trailer-section = *( field-line CRLF )
    // ==================== chunked-body 规则测试 ====================
    constexpr auto chunked_body_pass = make_pass_test<chunk_body>();
    constexpr auto chunked_body_fail = make_unpass_test<chunk_body>();
    static_assert(
        chunked_body_pass("0\r\n\r\n"_span)); // 最小合法结构。 仅仅last-chunk + CRLF
    // NOTE: 可不可以认为。当 chunk-size 为 0 时，就是最后一个chunk
    static_assert(chunked_body_pass("1\r\na\r\n0\r\n\r\n"_span)); // 单chunk+last-chunk
    static_assert(chunked_body_pass("2\r\nab\r\n"
                                    "3\r\nabc\r\n"
                                    "0\r\n\r\n"_span)); // 多chunk序列
    static_assert(chunked_body_pass("1;ext=val\r\na\r\n"
                                    "0;finish\r\n"
                                    "X-Trailer: value\r\n\r\n"_span)); // 带扩展和trailer
    static_assert(chunked_body_pass("A\r\n1234567890\r\n"
                                    "0\r\n"
                                    "Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==\r\n"
                                    "X-Status: Complete\r\n\r\n"_span)); // 多行trailer

    // 无效 chunked-body 测试
    static_assert(!chunked_body_pass(""_span));      // 空输入
    static_assert(!chunked_body_fail("0\r\n"_span)); // 缺少尾部CRLF
    static_assert(
        !chunked_body_pass("0\r\nX-Header: value\r\n"_span)); // trailer后缺少CRLF
    static_assert(!chunked_body_fail("1\r\na\r\n"_span));     // 缺少last-chunk
    static_assert(!chunked_body_fail(
        "1\r\na\r\n0\r\nInvalid-Trailer\r\n\r\n"_span)); // 非法trailer格式
    static_assert(!chunked_body_fail("G\r\ninvalid\r\n0\r\n\r\n"_span)); // 非法chunk-size

    // 验证chunk-data长度严格匹配
    static_assert(
        !chunked_body_fail("2\r\na\r\n"_span)); // 数据不足(需要2字节，只有1字节)
    static_assert(
        !chunked_body_fail("1\r\nabc\r\n"_span)); // 数据超额(需要1字节，有3字节)

    return 0;
}
// NOLINTEND