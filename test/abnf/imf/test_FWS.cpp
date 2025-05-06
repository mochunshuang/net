#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto FWS_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = FWS{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 标准FWS
        constexpr std::array<OCTET, 3> valid1{' ', '\t', ' '}; // 多个WSP
        static_assert(FWS_rule(valid1));
        // 带CRLF的FWS
        constexpr std::array<OCTET, 5> valid2{' ', '\r', '\n', '\t', ' '};
        static_assert(FWS_rule(valid2));
    }

    // 无效值
    {
        constexpr auto FWS_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = FWS{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        constexpr std::array<OCTET, 2> invalid1{'\r', '\n'};
        static_assert(!FWS_rule(invalid1));
    }

    return 0;
}
// NOLINTEND