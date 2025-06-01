#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    //   using chunk_data = abnf::one_or_more<abnf::OCTET>;
    // ==================== chunk_data 规则测试 ====================
    constexpr auto chunk_data_pass = make_pass_test<chunk_data>();
    // constexpr auto chunk_data_fail = make_unpass_test<chunk_data>();

    static_assert(chunk_data_pass(";name=value"_span));             // 基础格式
    static_assert(chunk_data_pass("; \t name \t = \t value"_span)); // 带空白
    static_assert(chunk_data_pass(";name"_span));                   // 无值
    static_assert(chunk_data_pass(";name=\"quoted\""_span));        // 引号值
    static_assert(chunk_data_pass(";n=v;n2=v2"_span));              // 多个扩展
    static_assert(chunk_data_pass(";name=value"_span));             // token值
    static_assert(chunk_data_pass(";name=\"\\\"quote\\\"\""_span)); // 转义引号

    static_assert(chunk_data_pass(";name=val|ue"_span)); // 非法字符

    static_assert(chunk_data_pass("name=value"_span)); // 缺少分号
    static_assert(chunk_data_pass(";=value"_span));    // 空名称

    static_assert(chunk_data_pass(";\"name\"=value"_span)); // 引号名称
    static_assert(chunk_data_pass(";name=value\x01"_span)); // 控制字符
    static_assert(chunk_data_pass(";name=val\"ue"_span));   // 未闭合引号

    // NOTE: chunk_data 回消耗 "\r\n"
    static_assert(chunk_data_pass("\r"_span));
    static_assert(chunk_data_pass("\n"_span));
    static_assert(chunk_data_pass("\r\n"_span));

    static_assert(chunk_data_pass(" "_span));

    // 失败
    static_assert(not chunk_data_pass(""_span));

    return 0;
}
// NOLINTEND