#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::http;

int main()
{
    // unsatisfied-range = "*/" complete-length
    { // error parse
        constexpr auto unsatisfied_range_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = unsatisfied_range{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(not unsatisfied_range_rule("a"_span));
        static_assert(not unsatisfied_range_rule("6a"_span));
        static_assert(not unsatisfied_range_rule("*/"_span));
    }
    constexpr auto unsatisfied_range_rule = [](const auto &array) constexpr {
        auto span = std::span{array};
        parser_ctx ctx = make_parser_ctx(span);
        assert(ctx.cur_index == 0);
        auto suc = unsatisfied_range{}(ctx);
        assert(ctx.cur_index == span.size());
        assert(ctx.done());
        return suc;
    };
    static_assert(unsatisfied_range_rule("*/0"_span));
    static_assert(unsatisfied_range_rule("*/01"_span));
    static_assert(unsatisfied_range_rule("*/010"_span));
    static_assert(unsatisfied_range_rule("*/010"_span));
    static_assert(unsatisfied_range_rule("*/999999"_span));

    return 0;
}
// NOLINTEND