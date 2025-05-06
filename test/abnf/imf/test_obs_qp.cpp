#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // obs-qp          =   "\" (%d0 / obs-NO-WS-CTL / LF / CR)
    // 有效边界值
    {
        constexpr auto obs_qp_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = obs_qp{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        constexpr std::array<OCTET, 2> valid0{'\\', '\x00'};
        static_assert(obs_qp_rule(valid0));
        constexpr std::array<OCTET, 2> valid1{'\\', '\x01'}; // 在obs_NO_WS_CTL
        static_assert(obs_qp_rule(valid1));
        constexpr std::array<OCTET, 2> valid2{'\\', '\n'}; // LF
        static_assert(obs_qp_rule(valid2));
        constexpr std::array<OCTET, 2> valid3{'\\', '\r'}; // CR
        static_assert(obs_qp_rule(valid3));
    }

    // 无效值
    {
        constexpr auto obs_qp_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = obs_qp{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        constexpr std::array<OCTET, 2> invalid1{'\\', ' '}; // 空格属于WSP但不在此规则
        static_assert(not obs_qp_rule(invalid1));
    }

    return 0;
}
// NOLINTEND