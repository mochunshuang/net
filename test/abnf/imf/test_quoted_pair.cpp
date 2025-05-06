#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // quoted-pair     =   ("\" (VCHAR / WSP)) / obs-qp
    // obs-qp          =   "\" (%d0 / obs-NO-WS-CTL / LF / CR)
    // 有效边界值
    {
        constexpr auto quoted_pair_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = quoted_pair{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 标准情况
        constexpr std::array<OCTET, 2> valid1{'\\', 'A'}; // VCHAR
        static_assert(quoted_pair_rule(valid1));

        constexpr std::array<OCTET, 2> valid2{'\\', ' '}; // WSP
        static_assert(quoted_pair_rule(valid2));

        // obs-qp 情况
        constexpr std::array<OCTET, 2> valid3{'\\', 0x00}; // 0字符
        static_assert(quoted_pair_rule(valid3));
        constexpr std::array<OCTET, 2> valid4{'\\', 0x7F}; // 127
        static_assert(quoted_pair_rule(valid4));

        constexpr std::array<OCTET, 2> invalid2{'\\', '\n'}; // LF在obs-qp
        static_assert(quoted_pair_rule(invalid2));
    }

    // 无效值
    {
        constexpr auto quoted_pair_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = quoted_pair{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        constexpr std::array<OCTET, 1> invalid1{'\\'}; // 只有反斜杠
        static_assert(!quoted_pair_rule(invalid1));
    }

    return 0;
}
// NOLINTEND