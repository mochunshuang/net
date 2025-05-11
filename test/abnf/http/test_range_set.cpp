#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // 合法测试用例
    {
        // range-set = range-spec *( OWS "," OWS range-spec )
        // range-spec = int-range / suffix-range / other-range
        // int-range = first-pos "-" [ last-pos ]
        constexpr auto range_set_rule = make_pass_test<range_set>();
        // 单个 range-spec
        constexpr auto s1 = "bytes=0-500"_span;
        static_assert(range_set_rule(s1));

        // 多个 range-spec 带 OWS
        constexpr auto s2 = "items=1-100, 200-300"_span;
        constexpr auto s3 = "sections=A-Z  ,   a-z"_span;
        static_assert(range_set_rule(s2));
        {
            static_assert(not make_rule_test<int_range>("items=1-100"_span).first);
            static_assert(not make_rule_test<suffix_range>("items=1-100"_span).first);
            static_assert(make_rule_test<other_range>("items=1-100"_span).first);
            static_assert(make_rule_test<other_range>("items=1-100"_span).second.done());
            //
            static_assert(make_rule_test<int_range>("200-300"_span).second.done());
            static_assert(not make_rule_test<suffix_range>("200-300"_span).second.done());
            static_assert(make_rule_test<other_range>("200-300"_span).second.done());
        }
        static_assert(range_set_rule(s3));

        // 无空格紧凑格式
        constexpr auto s4 = "bytes=0-500,1000-1500,2000-2500"_span;
        static_assert(range_set_rule(s4));

        // 混合制表符和空格
        constexpr auto s5 = "range=1-100\t, 200-300"_span;
        static_assert(range_set_rule(s5));
        {
            // 空输入
            constexpr auto s1 = ""_span;
            static_assert(not range_set_rule(s1));

            // 非法 range-spec
            constexpr auto s4 = "bytes=0-500,invalid"_span;
            static_assert(range_set_rule(s4));

            // 未闭合的 range-spec
            constexpr auto s5 = "sections=A-"_span;
            static_assert(range_set_rule(s5));
            {
                // TODO 是否限制 - 不属于 other_range。目前肯定是按照规则正常运行
                static_assert(make_rule_test<other_range>("="_span).second.done());
                static_assert(make_rule_test<other_range>("A"_span).second.done());
                static_assert(make_rule_test<other_range>("-"_span).second.done());
                static_assert(not make_rule_test<int_range>("A-"_span).second.done());
            }
        }
    }
    // 非法测试用例
    {
        constexpr auto range_set_rule = make_unpass_test<range_set>();

        // 连续逗号
        constexpr auto s2 = "bytes=0-500,,1000-1500"_span;
        assert(not range_set_rule(s2));

        // 逗号后无 range-spec
        constexpr auto s3 = "items=1-100, "_span;
        static_assert(range_set_rule(s3));

        // 纯 OWS
        constexpr auto s6 = "  \t  "_span;
        static_assert(not range_set_rule(s6));
    }
    // 边界测试
    {
        // 极大 range-set
        constexpr auto s1 = "bytes=0-100,200-300 , 400-500,600-700,800-900"_span;
        static_assert(make_pass_test<range_set>()(s1));

        // 单个字符 range-spec（假设允许）
        constexpr auto s2 = "key=A"_span;
        static_assert(make_pass_test<range_set>()(s2)); // 需确认 range-spec 规则
    }

    return 0;
}
// NOLINTEND