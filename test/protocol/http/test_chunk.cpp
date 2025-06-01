#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // chunk = chunk-size [ chunk-ext ] CRLF chunk-data CRLF
    // ==================== chunk 规则测试 ====================
    constexpr auto chunk_pass = make_pass_test<chunk>();
    constexpr auto chunk_fail = make_unpass_test<chunk>();

    // 有效 chunk 测试
    static_assert(chunk_pass("1\r\na\r\n"_span)); // 基础格式

    static_assert(chunk_pass("A;ext=val\r\n"
                             "1234567890\r\n"_span));        // 带扩展
    static_assert(chunk_pass("3;x\r\nabc\r\n"_span));        // 无值扩展
    static_assert(chunk_pass("2;x=y\r\nab\r\n"_span));       // 带等号扩展
    static_assert(chunk_pass("0\r\n\r\n"_span));             // 零大小块
    static_assert(chunk_pass("4;x=\"y\"\r\nabcd\r\n"_span)); // 引号扩展值

    static_assert(chunk_pass("1 \t ; \t x \t = \t y\r\na\r\n"_span)); // 空白处理
    static_assert(!chunk_fail(
        "1 \t ; \t x \t = \t y \t \r\na\r\n"_span)); // //NOTE:chunk_ext_val 不能转义
    {
        // \t ; \t x \t = \t y \t
        static_assert(make_unpass_test<chunk_ext>()(" \t ; \t x \t = \t y \t "_span));
    }

    // 无效 chunk 测试
    static_assert(chunk_pass("1\r\nabc\r\n"_span)); // NOTE: 数据长度不匹配.【但是不校验】

    static_assert(!chunk_fail("G\r\na\r\n"_span)); // 非法大小

    static_assert(!chunk_fail("1;x=y\r\na"_span));         // 缺少结尾CRLF
    static_assert(!chunk_fail("1\n\ra\n\r"_span));         // 无效行尾
    static_assert(!chunk_fail("1 ext=val\r\na\r\n"_span)); // 缺少分号
    static_assert(!chunk_fail("1;x=\x01\r\na\r\n"_span));  // 非法扩展字符

    static_assert(chunk_pass("1\r\n\x00\r\n"_span)); // NOTE: 空字符数据【但是不校验】

    return 0;
}
// NOLINTEND