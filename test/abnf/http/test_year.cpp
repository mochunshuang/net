#include "../test_head.hpp"

#include <cassert>
#include <iostream>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    TEST("base") = [] {
        constexpr auto year_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = year{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        constexpr auto y = "9999"_span;
        static_assert(year_rule(y));
    };
    {
        constexpr auto year_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = year{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };

        {
            constexpr auto y = "999"_span;
            static_assert(not year_rule(y));
        }
        {
            constexpr auto y = "999a"_span;
            static_assert(not year_rule(y));
        }
    }
    std::cout << "main done\n";
    return 0;
}
// NOLINTEND