#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // HTTP-date = IMF-fixdate / obs-date
    constexpr auto http_date_pass = make_pass_test<mcs::abnf::http::HTTP_date>();
    constexpr auto http_date_fail = make_unpass_test<mcs::abnf::http::HTTP_date>();
    static_assert(http_date_pass("Mon, 07 Aug 2023 12:34:56 GMT"_span));  // IMF格式
    static_assert(http_date_pass("Sunday, 06-Nov-94 08:49:37 GMT"_span)); // 过时格式
    static_assert(not http_date_fail("Invalid Date Format"_span));

    // 测试过时日期格式的月份缩写
    static_assert(not http_date_fail("Fri, 31-Dec-99 23:59:59 GMT"_span));
    return 0;
}
// NOLINTEND