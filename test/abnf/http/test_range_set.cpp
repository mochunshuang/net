#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 单个 range-spec
        constexpr auto s1 = "bytes=0-500"_span;
        static_assert(range_set(s1));

        // 多个 range-spec 带 OWS
        constexpr auto s2 = "items=1-100, 200-300"_span;
        constexpr auto s3 = "sections=A-Z  ,   a-z"_span;
        static_assert(range_set(s2));
        {
            static_assert(not int_range("items=1-100"_span));
            static_assert(not suffix_range("items=1-100"_span));
            static_assert(other_range("items=1-100"_span));
            //
            static_assert(int_range("200-300"_span));
            static_assert(not suffix_range("200-300"_span));
            static_assert(other_range("200-300"_span));
        }
        static_assert(range_set(s3));

        // 无空格紧凑格式
        constexpr auto s4 = "bytes=0-500,1000-1500,2000-2500"_span;
        static_assert(range_set(s4));

        // 混合制表符和空格
        constexpr auto s5 = "range=1-100\t,\t200-300"_span;
        static_assert(range_set(s5));
    }
    // 非法测试用例
    {
        // 空输入
        constexpr auto s1 = ""_span;
        static_assert(not range_set(s1));

        // 连续逗号
        constexpr auto s2 = "bytes=0-500,,1000-1500"_span;
        assert(not range_set(s2));

        // 逗号后无 range-spec
        constexpr auto s3 = "items=1-100, "_span;
        static_assert(not range_set(s3));

        // 非法 range-spec
        constexpr auto s4 = "bytes=0-500,invalid"_span;
        static_assert(range_set(s4));

        // 未闭合的 range-spec
        constexpr auto s5 = "sections=A-"_span;
        static_assert(range_set(s5));
        {
            static_assert(not int_range("sections=A-"_span));
            static_assert(not suffix_range("sections=A-"_span));
            static_assert(other_range("sections=A-"_span));
        }

        // 纯 OWS
        constexpr auto s6 = "  \t  "_span;
        static_assert(not range_set(s6));
    }
    // 边界测试
    {
        // 极大 range-set
        constexpr auto s1 = "bytes=0-100,200-300 , 400-500,600-700,800-900"_span;
        static_assert(range_set(s1));

        // 单个字符 range-spec（假设允许）
        constexpr auto s2 = "key=A"_span;
        static_assert(range_set(s2)); // 需确认 range-spec 规则
    }

    return 0;
}
// NOLINTEND