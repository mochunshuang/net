#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // entity-tag = [ weak ] opaque-tag
    // opaque-tag = DQUOTE *etagc DQUOTE
    constexpr auto entity_tag_pass = make_pass_test<mcs::abnf::http::entity_tag>();
    constexpr auto entity_tag_fail = make_unpass_test<mcs::abnf::http::entity_tag>();

    // 有效 entity-tag 测试
    static_assert(entity_tag_pass("\"abc123\""_span)); // 基础形式
    static_assert(entity_tag_pass("W/\"xyz\""_span));  // 弱校验标记

    static_assert(entity_tag_pass("\"!#$%&\'()*+-.^_`|~\""_span)); // 全符号组合

    // 无效 entity-tag 测试
    static_assert(entity_tag_fail("W/\"a\\\"b\""_span)); // 带转义引号
    {
        // opaque-tag = DQUOTE *etagc DQUOTE
        static_assert(not make_unpass_test<mcs::abnf::http::entity_tag>()("a\\\"b"_span));
    }
    static_assert(not entity_tag_fail("W/123"_span));       // 缺少引号
    static_assert(not entity_tag_fail("\"unclosed"_span));  // 引号未闭合
    static_assert(not entity_tag_fail("weak\"123\""_span)); // 未使用W/前缀
    static_assert(not entity_tag_fail("W/\"\x01\""_span));  // 控制字符
    return 0;
}
// NOLINTEND