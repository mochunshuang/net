#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // last-chunk = 1*"0" [ chunk-ext ] CRLF
    // ==================== last-chunk 规则测试 ====================
    constexpr auto last_chunk_pass = make_pass_test<last_chunk>();
    constexpr auto last_chunk_fail = make_unpass_test<last_chunk>();

    // 有效 last-chunk 测试
    static_assert(last_chunk_pass("0\r\n"_span));              // 最小合法格式
    static_assert(last_chunk_pass("000\r\n"_span));            // 多个零
    static_assert(last_chunk_pass("0;ext=val\r\n"_span));      // 带简单扩展
    static_assert(last_chunk_pass("0;name\r\n"_span));         // 无值扩展
    static_assert(last_chunk_pass("0;x=\"quoted\"\r\n"_span)); // 引号扩展值
    // static_assert(last_chunk_pass("0 \t ; \t n \t = \t v \t \r\n"_span)); // 带空白
    static_assert(last_chunk_pass("000;finish=true\r\n"_span)); // 多个零带扩展
    static_assert(last_chunk_pass("0;n=v;n2=v2\r\n"_span));     // 多个扩展

    static_assert(last_chunk_pass("0;n=v|ue\r\n"_span)); // 非法扩展字符

    // 无效 last-chunk 测试
    static_assert(!last_chunk_pass(""_span));             // 空字符串
    static_assert(!last_chunk_fail("1\r\n"_span));        // 非零数字
    static_assert(!last_chunk_fail("0\n\r"_span));        // 无效行尾(LFCR)
    static_assert(!last_chunk_fail("0"_span));            // 缺少CRLF
    static_assert(!last_chunk_fail("0 \r\n"_span));       // 尾部空白(规则要求紧接CRLF)
    static_assert(!last_chunk_fail("0;=value\r\n"_span)); // 空扩展名

    static_assert(!last_chunk_fail("0;\"n\"=v\r\n"_span)); // 引号扩展名
    static_assert(!last_chunk_fail("0ext=val\r\n"_span));  // 缺少分号

    return 0;
}
// NOLINTEND