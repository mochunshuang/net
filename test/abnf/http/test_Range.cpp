#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // Range = ranges-specifier
    // ranges-specifier = range-unit "=" range-set
    constexpr auto range_pass = make_pass_test<mcs::abnf::http::Range>();
    constexpr auto range_fail = make_unpass_test<mcs::abnf::http::Range>();

    static_assert(range_pass("bytes=0-499"_span));          // 标准字节范围
    static_assert(range_pass("custom=1-100,200-300"_span)); // 自定义range-unit
    static_assert(range_pass("bytes=500-"_span));           // 无效结束位置
    static_assert(range_pass("bytes=-0"_span));             // 无效起始位置

    return 0;
}
// NOLINTEND