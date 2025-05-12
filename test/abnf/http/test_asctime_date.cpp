#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // asctime-date = day-name SP date3 SP time-of-day SP year
    constexpr auto asctime_date_pass = make_pass_test<mcs::abnf::http::asctime_date>();
    constexpr auto asctime_date_fail = make_unpass_test<mcs::abnf::http::asctime_date>();

    // 有效日期测试
    static_assert(asctime_date_pass("Wed Jun 30 21:49:08 2023"_span)); // 标准格式
    static_assert(asctime_date_pass("Mon Feb 29 00:00:00 2024"_span)); // 闰年日期
    static_assert(asctime_date_pass("Sun Dec 31 23:59:59 9999"_span)); // 边界年

    static_assert(asctime_date_pass("Wed Jun 30 25:00:00 2023"_span)); // 非法时间

    // 无效日期测试
    static_assert(
        not asctime_date_fail("Wednesday Jun 30 21:49:08 2023"_span));   // 全称星期
    static_assert(not asctime_date_fail("Wed Jun 30 21:49:08 23"_span)); // 短年份

    static_assert(
        not asctime_date_fail("InvalidMon 13 99:99:99 99999"_span)); // 完全错误格式

    return 0;
}
// NOLINTEND