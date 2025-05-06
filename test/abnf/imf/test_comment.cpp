#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto comment_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::comment{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 简单注释
        constexpr std::array<OCTET, 2> valid1{'(', ')'};
        static_assert(comment_rule(valid1));
        // 带内容的注释
        constexpr std::array<OCTET, 3> valid2{'(', 'A', ')'};
        static_assert(comment_rule(valid2));
        // 嵌套注释
        constexpr std::array<OCTET, 6> valid3{'(', '(', ')', '(', ')', ')'};
        static_assert(comment_rule(valid3));
        // comment情况

        constexpr auto com_case2 = "(abndacsd+*a)"_span;
        static_assert(comment_rule(com_case2));

        constexpr auto com_case3 = "(a(125asd)c(sd+(8557)*a)"_span;
        static_assert(comment_rule(com_case2));

        constexpr auto com_case4 = "( 1 9   2)"_span;
        static_assert(comment_rule(com_case4));
    }

    // 无效值
    {
        constexpr auto ctest_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::comment{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!ctest_rule(")"_span)); // 41 应无效

        // 无效（未闭合）
        constexpr std::array<OCTET, 3> invalid1{'(', 'A', ' '};
        static_assert(!ctest_rule(invalid1));

        static_assert(!ctest_rule("((())"_span));
        static_assert(ctest_rule("()(    ))"_span));
    }

    return 0;
}
// NOLINTEND