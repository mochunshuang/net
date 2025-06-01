#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{

    // transfer-parameter = token BWS "=" BWS ( token / quoted-string )
    // transfer-coding = token *( OWS ";" OWS transfer-parameter )
    // Transfer-Encoding = [ transfer-coding *( OWS "," OWS transfer-coding ) ]
    // ==================== Transfer-Encoding 规则测试 ====================
    // ==================== Transfer-Encoding 规则测试 ====================
    constexpr auto transfer_encoding_pass = make_pass_test<Transfer_Encoding>();
    constexpr auto transfer_encoding_fail = make_unpass_test<Transfer_Encoding>();

    // 有效 Transfer-Encoding 测试
    static_assert(transfer_encoding_pass("chunked"_span));       // 单个有效值
    static_assert(transfer_encoding_pass("gzip, chunked"_span)); // 逗号分隔多个值
    static_assert(
        transfer_encoding_pass("compress\t,\t \tdeflate"_span)); // 含制表符和空格的OWS
    static_assert(transfer_encoding_pass("custom;param=value"_span));  // 带token参数
    static_assert(transfer_encoding_pass("ext;name=\"quoted\""_span)); // 带引号字符串参数
    static_assert(transfer_encoding_pass("gzip; q=1.0"_span));         // 含数字值的参数
    static_assert(
        transfer_encoding_pass("chunked, gzip, deflate"_span));       // 三个值逗号分隔
    static_assert(transfer_encoding_pass("trailers, chunked"_span));  // RFC规范定义值
    static_assert(transfer_encoding_pass("x-bzip2;version=2"_span));  // 带版本参数
    static_assert(transfer_encoding_pass("custom; p1=a; p2=b"_span)); // 多个参数

    // 无效 Transfer-Encoding 测试
    static_assert(transfer_encoding_fail("chunked gzip"_span));      // 缺少逗号分隔符
    static_assert(transfer_encoding_fail(",chunked"_span));          // 起始逗号无效
    static_assert(transfer_encoding_fail("chunked,"_span));          // 结尾逗号无效
    static_assert(transfer_encoding_fail("invalid;=value"_span));    // 空参数名
    static_assert(transfer_encoding_fail("chunked; \x01=val"_span)); // 非法控制字符
    static_assert(transfer_encoding_fail("gzip; param=\""_span));    // 未闭合的引号
    static_assert(transfer_encoding_fail("(invalid)"_span));         // 非法括号字符
    static_assert(transfer_encoding_fail("gzip; q=a=b"_span));       // 多个等号
    static_assert(transfer_encoding_fail("deflate;@"_span));         // 非法特殊字符
    static_assert(transfer_encoding_fail("chunked;;param"_span));    // 空分号段

    return 0;
}
// NOLINTEND