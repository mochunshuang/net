#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 有效测试用例
    {
        constexpr auto s1 = "00:00:00"_span;
        static_assert(time_of_day(s1));

        constexpr auto s2 = "23:59:59"_span;
        static_assert(time_of_day(s2));

        constexpr auto s3 = "12:34:56"_span;
        static_assert(time_of_day(s3));
    }

    // 无效测试用例
    {
        // 格式错误
        constexpr auto s1 = "12:34:5"_span; // 长度不足
        static_assert(not time_of_day(s1));

        constexpr auto s2 = "12:34:567"_span; // 长度超限
        static_assert(not time_of_day(s2));

        constexpr auto s3 = "12-34-56"_span; // 错误分隔符
        static_assert(not time_of_day(s3));

        // 非法数值
        constexpr auto s4 = "24:00:00"_span; // 小时超限
        static_assert(time_of_day(s4));

        constexpr auto s5 = "12:60:00"_span; // 分钟超限
        static_assert(time_of_day(s5));

        constexpr auto s6 = "12:30:60"_span; // 秒超限
        static_assert(time_of_day(s6));

        // 非数字字符
        constexpr auto s7 = "1a:30:45"_span; // 小时非数字
        static_assert(not time_of_day(s7));

        constexpr auto s8 = "12:3b:45"_span; // 分钟非数字
        static_assert(not time_of_day(s8));

        constexpr auto s9 = "12:30:4c"_span; // 秒非数字
        static_assert(not time_of_day(s9));
    }

    // 边界测试
    {
        constexpr auto s1 = "00:00:00"_span; // 下限
        static_assert(time_of_day(s1));

        constexpr auto s2 = "23:59:59"_span; // 上限
        static_assert(time_of_day(s2));

        constexpr auto s3 = "00:00:60"_span; // 秒边界外
        static_assert(time_of_day(s3));
    }
    return 0;
}
// NOLINTEND