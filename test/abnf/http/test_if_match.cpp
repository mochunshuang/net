#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // If-Match = "*" / [ entity-tag *( OWS "," OWS entity-tag ) ]
    constexpr auto if_match_pass = make_pass_test<mcs::abnf::http::If_Match>();
    constexpr auto if_match_fail = make_unpass_test<mcs::abnf::http::If_Match>();

    static_assert(if_match_pass("*"_span));                     // 通配符
    static_assert(if_match_pass("\"abc123\", W/\"xyz\""_span)); // 多实体标签

    // 测试带参数的编码
    static_assert(if_match_fail("\"\\\"quoted\\\"\""_span)); // 转义引号
    static_assert(if_match_fail("\"unclosed"_span));         // 未闭合标签
    static_assert(if_match_fail("*, \"etag\""_span));        // 混合通配符

    return 0;
}
// NOLINTEND