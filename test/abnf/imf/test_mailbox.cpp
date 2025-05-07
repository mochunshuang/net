#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto mailbox_rule = [](const auto &span) constexpr {
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mailbox{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 标准邮箱
        static_assert(mailbox_rule("user@example.com"_span));
        static_assert(mailbox_rule(R"("John Doe" <john@example.com>)"_span));

        // 带CFWS
        constexpr auto valid1 = R"( (comment)user(another)@example.com)"_span;
        static_assert(mailbox_rule(valid1));

        // NOTE: domain 允许 没有 .com 等
        static_assert(mailbox_rule("invalid@domain"_span));
    }

    // 无效值
    {
        constexpr auto mailbox_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mailbox{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!mailbox_rule("."_span));
    }

    return 0;
}
// NOLINTEND