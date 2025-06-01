#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // chunk-ext = *( BWS ";" BWS chunk-ext-name [ BWS "=" BWS chunk-ext-val ] )
    // ==================== chunk-ext 规则测试 ====================
    constexpr auto chunk_ext_pass = make_pass_test<chunk_ext>();
    constexpr auto chunk_ext_fail = make_unpass_test<chunk_ext>();

    // 有效 chunk-ext 测试
    static_assert(chunk_ext_pass(";name=value"_span));                   // 基础格式
    static_assert(chunk_ext_pass("; \t name \t = \t value"_span));       // 带空白
    static_assert(chunk_ext_pass("  ; \t name \t = \t value"_span));     // 带空白
    static_assert(chunk_ext_pass(" \t ; \t name \t = \t value"_span));   // 带空白
    static_assert(chunk_ext_pass(";name"_span));                         // 无值
    static_assert(chunk_ext_pass(";name=\"quoted\""_span));              // 引号值
    static_assert(chunk_ext_pass(";n=v;n2=v2"_span));                    // 多个扩展
    static_assert(chunk_ext_pass(";name=value"_span));                   // token值
    static_assert(chunk_ext_pass(";name=\"\\\"quote\\\"\""_span));       // 转义引号
    static_assert(chunk_ext_pass(";name=\"\\\"quote\\r\\n\\\"\""_span)); // 转义引号

    static_assert(chunk_ext_pass(";name=val|ue"_span)); // 非法字符

    // 无效 chunk-ext 测试
    static_assert(chunk_ext_fail("name=value"_span)); // 缺少分号
    static_assert(chunk_ext_fail(";=value"_span));    // 空名称

    static_assert(chunk_ext_fail(";\"name\"=value"_span)); // 引号名称
    static_assert(chunk_ext_fail(";name=value\x01"_span)); // 控制字符
    static_assert(chunk_ext_fail(";name=val\"ue"_span));   // 未闭合引号

    // NOTE: 独立于 \r\n
    static_assert(chunk_ext_fail("\r"_span));
    static_assert(chunk_ext_fail("\r\n"_span)); // 未闭合引号
    return 0;
}
// NOLINTEND