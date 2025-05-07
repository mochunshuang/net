#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto address_list_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = address_list{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 多地址
        constexpr auto valid1 = "user1@a.com, user2@b.com"_span;
        static_assert(address_list_rule(valid1));

        // 带组地址
        constexpr auto valid2 = "Group: member1@a.com, member2@b.com;"_span;
        static_assert(address_list_rule(valid2));
    }

    // 无效值
    {
        constexpr auto address_list_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = address_list{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        constexpr auto valid1 = "user@domain.com>"_span;
        static_assert(address_list_rule(valid1));
    }

    return 0;
}
// NOLINTEND