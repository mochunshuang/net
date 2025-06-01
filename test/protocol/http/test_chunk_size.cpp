#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{

    // ==================== chunk-size 规则测试 ====================
    constexpr auto chunk_size_pass = make_pass_test<chunk_size>();
    constexpr auto chunk_size_fail = make_unpass_test<chunk_size>();

    // 有效 chunk-size 测试
    static_assert(chunk_size_pass("0"_span));   // 最小合法值
    static_assert(chunk_size_pass("A"_span));   // 大写十六进制
    static_assert(chunk_size_pass("f"_span));   // 小写十六进制
    static_assert(chunk_size_pass("10"_span));  // 多位数
    static_assert(chunk_size_pass("FFF"_span)); // 最大长度测试
    static_assert(chunk_size_pass("000"_span)); // 允许前导零

    // 无效 chunk-size 测试
    static_assert(!chunk_size_pass(""_span));    // 空字符串
    static_assert(!chunk_size_fail("G"_span));   // 非法十六进制字符
    static_assert(!chunk_size_fail("-1"_span));  // 负号
    static_assert(chunk_size_fail("0x10"_span)); // 包含非HEXDIG字符
    static_assert(chunk_size_fail("12G"_span));  // 非法字符结尾

    // NOTE: 终止于 ";"
    static_assert(!chunk_size_fail(";"_span)); // 空字符串
    return 0;
}
// NOLINTEND