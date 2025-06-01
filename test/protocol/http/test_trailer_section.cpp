#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // trailer-section = *( field-line CRLF )
    // ==================== trailer-section 规则测试 ====================
    constexpr auto trailer_pass = make_pass_test<trailer_section>();
    constexpr auto trailer_fail = make_unpass_test<trailer_section>();

    // 有效 trailer-section 测试
    static_assert(trailer_pass("X-Trailer: value"
                               "\r\n"_span)); // 单个字段

    static_assert(trailer_pass("Content-MD5: Q2hlY2sgSW50ZWdyaXR5IQ==\r\n"
                               "X-Status: Complete\r\n"
                               ""_span)); // 多个字段
    static_assert(trailer_pass("X-Whitespace:   value  \t \r\n"
                               ""_span)); // 带空白字符
    static_assert(trailer_pass("X-Empty:\r\n"
                               ""_span)); // 空字段值
    static_assert(trailer_pass("X-Control\u0001Char: value\r\n"
                               ""_span)); // 非法控制字符,在名字上是允许的
    static_assert(trailer_pass(""_span)); // 完全为空

    // 无效 trailer-section 测试
    static_assert(trailer_fail("\r\n"_span)); // NOTE: field-line 不能为空 空 trailer
    static_assert(trailer_fail("X-Trailer: value\r\n"
                               "Invalid Field\r\n" // 缺少冒号
                               ""_span));
    static_assert(trailer_fail("X-Trailer: value\r\n"
                               "X-No-CRLF: value"_span // 缺少尾部CRLF
                               ));

    static_assert(trailer_fail("X-Trailer: val\u000Bue\r\n" // 垂直制表符
                               ""_span));

    // ==================== trailer-section 规则补充测试 ====================
    static_assert(trailer_fail("X-Header: val\r\nue\r\n"_span)); // 非法折叠行(需空格/tab)
    static_assert(trailer_pass("X-Header: value\r\nX-Folded: continuation\r\n"_span));
    static_assert(trailer_pass("X-Header: value\r\n  X-Folded: continuation\r\n"_span));
    return 0;
}
// NOLINTEND