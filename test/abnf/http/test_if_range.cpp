#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // If-Range = entity-tag / HTTP-date
    constexpr auto if_range_pass = make_pass_test<mcs::abnf::http::If_Range>();
    constexpr auto if_range_fail = make_unpass_test<mcs::abnf::http::If_Range>();
    static_assert(if_range_pass("\"abc123\""_span));                    // 实体标签
    static_assert(if_range_pass("Wed, 21 Oct 2023 07:28:00 GMT"_span)); // 日期格式
    static_assert(not if_range_fail("123456"_span));                    // 纯数字

    return 0;
}
// NOLINTEND