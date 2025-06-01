#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // reason-phrase = 1*( HTAB / SP / VCHAR / obs-text )
    // ==================== reason-phrase 规则测试 ====================
    constexpr auto reason_phrase_pass = make_pass_test<reason_phrase>();
    constexpr auto reason_phrase_fail = make_unpass_test<reason_phrase>();

    // 有效 reason-phrase 测试
    static_assert(reason_phrase_pass("a"_span));                    // 最小长度
    static_assert(reason_phrase_pass("Document follows"_span));     // 常规短语
    static_assert(reason_phrase_pass("  Leading spaces"_span));     // 前导空格
    static_assert(reason_phrase_pass("Trailing spaces  "_span));    // 尾部空格
    static_assert(reason_phrase_pass("Mixed  \t whitespace"_span)); // 混合空白(HTAB+SP)
    static_assert(reason_phrase_pass("Valid!@#$%^&*()"_span));      // 可见ASCII字符
    static_assert(reason_phrase_pass("\xC2\xA9 Copyright"_span)); // obs-text (扩展ASCII)
    static_assert(reason_phrase_pass("Bad\x7F"_span));            // DEL字符
    static_assert(reason_phrase_pass("\x80\x81"_span));           // UTF-8序列
    static_assert(reason_phrase_pass("你好，世界"_span));         // UTF-8序列

    // 无效 reason-phrase 测试
    static_assert(!reason_phrase_pass(""_span));             // 空字符串
    static_assert(!reason_phrase_fail("\r\n"_span));         // 控制字符
    static_assert(reason_phrase_fail("Invalid\u0007"_span)); // BEL控制字符

    return 0;
}
// NOLINTEND