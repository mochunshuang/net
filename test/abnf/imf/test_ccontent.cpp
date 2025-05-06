#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto ccontent_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::ccontent{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // ctext情况
        static_assert(ccontent_rule("!"_span));
        // quoted-pair情况
        constexpr std::array<OCTET, 2> qp_case{'\\', 'A'};
        static_assert(ccontent_rule(qp_case));
        // comment情况
        constexpr std::array<OCTET, 2> com_case{'(', ')'}; // 空注释
        static_assert(ccontent_rule(com_case));

        constexpr auto com_case2 = "(abndacsd+*a)"_span;
        static_assert(ccontent_rule(com_case2));
    }

    // 无效值
    {
        constexpr auto ctest_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::ccontent{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!ctest_rule(")"_span)); // 41 应无效
    }

    return 0;
}
// NOLINTEND