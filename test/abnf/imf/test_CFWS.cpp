#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto CFWS_rule = [](const auto &span) constexpr {
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = CFWS{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 纯注释
        static_assert(CFWS_rule("(comment)"_span));

        // 混合空白
        constexpr auto valid1 = "  \t (comment)\r\n "_span;
        static_assert(CFWS_rule(valid1));
    }

    // 无效值
    {
        constexpr auto CFWS_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = CFWS{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!CFWS_rule("."_span));
    }

    return 0;
}
// NOLINTEND