#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{

    // IMF-fixdate = day-name "," SP date1 SP time-of-day SP GMT
    constexpr auto imf_fixdate_pass = make_pass_test<mcs::abnf::http::IMF_fixdate>();
    constexpr auto imf_fixdate_fail = make_unpass_test<mcs::abnf::http::IMF_fixdate>();
    // 测试过期的日期格式
    static_assert(imf_fixdate_pass("Mon, 07 Aug 2023 12:34:56 GMT"_span)); // RFC 5322格式
    static_assert(imf_fixdate_pass("Fri, 31 Feb 2023 00:00:00 GMT"_span)); // 无效日期
    static_assert(imf_fixdate_pass("Sat, 29 Oct 1994 19:43:31 GMT"_span));
    
    static_assert(
        not imf_fixdate_fail("Monday, 07-Aug-23 12:34:56 GMT"_span));         // 全称星期
    static_assert(not imf_fixdate_fail("Mon 07 Aug 2023 12:34:56 UTC"_span)); // 错误时区

    return 0;
}
// NOLINTEND