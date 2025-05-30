#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto obs_NO_WS_CTL_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = obs_NO_WS_CTL{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        static_assert(obs_NO_WS_CTL_rule("\x01"_span));
        static_assert(obs_NO_WS_CTL_rule("\x08"_span));
        static_assert(obs_NO_WS_CTL_rule("\x0B"_span));
        static_assert(obs_NO_WS_CTL_rule("\x7F"_span));
    }

    // 无效值
    {
        constexpr auto obs_NO_WS_CTL_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = obs_NO_WS_CTL{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(not obs_NO_WS_CTL_rule("\x09"_span));
        static_assert(not obs_NO_WS_CTL_rule("\x0D"_span));
    }

    return 0;
}
// NOLINTEND