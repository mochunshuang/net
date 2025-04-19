#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // 基础合法格式
        constexpr auto s1 = "bytes=0-500"_span;
        constexpr auto s2 = "items=1-100,200-300"_span;
        static_assert(ranges_specifier(s1));
        static_assert(ranges_specifier(s2));

        // 特殊 range-unit（根据 token 规则）
        constexpr auto s3 = "custom~unit=1-99"_span;
        constexpr auto s4 = "unit_v1.2=100-"_span; // 开放结尾范围
        static_assert(ranges_specifier(s3));
        static_assert(ranges_specifier(s4));

        // 紧凑格式
        constexpr auto s5 = "sections=A-Z,a-z"_span;
        static_assert(ranges_specifier(s5));
    }
    // 非法测试用例
    {
        // 缺少等号
        constexpr auto s1 = "bytes0-500"_span;
        static_assert(not ranges_specifier(s1));

        // 非法 range-unit
        constexpr auto s2 = "unit name=1-100"_span; // 空格
        constexpr auto s3 = "invalid@unit=0-"_span; // 非法字符
        static_assert(not ranges_specifier(s2));
        static_assert(not ranges_specifier(s3));

        // 等号后非法 range-set
        constexpr auto s4 = "bytes=0-500,,"_span;       // 空 range-spec
        constexpr auto s5 = "items=1-100;200-300"_span; // 错误分隔符
        assert(not ranges_specifier(s4));
        static_assert(ranges_specifier(s5));
        {
            // ranges-specifier = range-unit "=" range-set
            static_assert(range_unit("items"_span));
            static_assert(range_set("1-100;200-300"_span));
        }

        // 空输入
        constexpr auto s6 = ""_span;
        static_assert(not ranges_specifier(s6));
    }
    // 边界测试
    {
        // 极长组合
        constexpr auto s1 = "abcdefghijk=0-999,1000-1999,2000-2999"_span;
        static_assert(ranges_specifier(s1));

        // 最小合法单元
        constexpr auto s2 = "a=0-0"_span;
        static_assert(ranges_specifier(s2));

        // 等号后无内容
        constexpr auto s3 = "unit="_span;
        static_assert(not ranges_specifier(s3));
        {
            static_assert(not range_set(""_span));
        }
    }

    return 0;
}
// NOLINTEND