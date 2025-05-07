#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto angle_addr_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::angle_addr{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 标准格式
        constexpr auto valid1 = "<user@domain.com>"_span;
        static_assert(angle_addr_rule(valid1));

        // 带CFWS
        constexpr auto valid2 = "<(comment)user@domain>"_span;
        static_assert(angle_addr_rule(valid2));
    }

    // 无效值
    {
        constexpr auto angle_addr_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::imf::angle_addr{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        constexpr auto valid1 = "user@domain.com>"_span;
        static_assert(!angle_addr_rule(valid1));
    }

    return 0;
}
// NOLINTEND