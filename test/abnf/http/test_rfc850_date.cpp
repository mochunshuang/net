#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 基础合法格式
        constexpr auto s1 = "Monday, 01-Jan-99 12:34:56 GMT"_span;
        static_assert(rfc850_date(s1));
        {
            // return sp[index + 1] == SP && date2(sp.subspan(index + 2, 9)) && // NOLINT
            //        sp[index + 12] == SP &&
            //        time_of_day(sp.subspan(index + 13, 8)) &&            // NOLINT
            //        GMT(sp[index + 21], sp[index + 22], sp[index + 23]); // NOLINT
            constexpr auto index = 6;
            static_assert(s1[index] == ',');
            static_assert(s1[index + 1] == SP && date2(s1.subspan(index + 2, 9)));
            static_assert(s1[index + 11] == SP && time_of_day(s1.subspan(index + 12, 8)));
            static_assert(s1[index + 20] == SP &&
                          GMT(s1[index + 21], s1[index + 22], s1[index + 23]));
        }

        // 边界日期和时间
        constexpr auto s2 = "Friday, 31-Dec-00 23:59:59 GMT"_span;
        static_assert(rfc850_date(s2));

        // 闰年日期（需确认具体实现是否验证）
        constexpr auto s3 = "Wednesday, 29-Feb-24 00:00:00 GMT"_span;
        assert(rfc850_date(s3));

        // 月份缩写测试
        constexpr auto s4 = "Sunday, 15-Jul-23 08:15:30 GMT"_span;
        static_assert(rfc850_date(s4));
    }
    // 非法测试用例
    {
        // 星期名称拼写错误
        constexpr auto s1 = "Mondai, 01-Jan-99 12:34:56 GMT"_span;
        static_assert(not rfc850_date(s1));

        // 错误的分隔符
        constexpr auto s2 = "Monday, 01/Jan/99 12:34:56 GMT"_span;
        static_assert(not rfc850_date(s2));

        // 无效日期（30-Feb）
        constexpr auto s3 = "Tuesday, 30-Feb-21 08:00:00 GMT"_span;
        static_assert(rfc850_date(s3));

        // 月份缩写错误
        constexpr auto s4 = "Thursday, 15-Abr-22 12:34:56 GMT"_span;
        static_assert(not rfc850_date(s4));

        // 时间超限（25:00:00）
        constexpr auto s5 = "Sunday, 05-Jul-99 25:00:00 GMT"_span;
        static_assert(rfc850_date(s5));

        // 缺少 GMT
        constexpr auto s6 = "Saturday, 10-Sep-99 12:34:56"_span;
        static_assert(not rfc850_date(s6));

        // 格式空格错误
        constexpr auto s7 = "Monday,01-Jan-99 12:34:56 GMT"_span; // 逗号后无空格
        static_assert(not rfc850_date(s7));
    }
    // 边界测试
    {
        // 最小日期
        constexpr auto s1 = "Monday, 01-Jan-00 00:00:00 GMT"_span;
        static_assert(rfc850_date(s1));

        // 最大两位数年份
        constexpr auto s2 = "Friday, 31-Dec-99 23:59:59 GMT"_span;
        static_assert(rfc850_date(s2));

        // 无效月份缩写长度
        constexpr auto s3 = "Wednesday, 01-January-99 12:34:56 GMT"_span; // 月份应为3字母
        static_assert(not rfc850_date(s3));
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND