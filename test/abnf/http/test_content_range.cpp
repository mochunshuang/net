#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Content-Range = range-unit SP ( range-resp / unsatisfied-range )
    constexpr auto content_range_pass = make_pass_test<mcs::abnf::http::Content_Range>();

    static_assert(content_range_pass("bytes 0-499/1234"_span)); // 具体范围
    static_assert(content_range_pass("bytes */5678"_span));     // 未满足范围

    static_assert(content_range_pass("bytes 500-499/1234"_span)); // 无效区间

    // 测试复杂内容范围
    static_assert(content_range_pass("bytes 42-1234/*"_span)); // 未知总长度

    return 0;
}
// NOLINTEND