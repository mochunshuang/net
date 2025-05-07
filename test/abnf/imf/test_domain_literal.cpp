#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto domain_literal_rule = [](const auto &span) constexpr {
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = domain_literal{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // IPv6地址
        constexpr auto valid1 = "[IPv6:2001:db8::1]"_span;
        static_assert(domain_literal_rule(valid1));

        // IP地址
        constexpr auto valid2 = "[192.168.0.1]"_span;
        static_assert(domain_literal_rule(valid2));

        // domain-literal  =   [CFWS] "[" *([FWS] dtext) [FWS] "]" [CFWS]
        static_assert(domain_literal_rule("[invalid]"_span));
        {
            constexpr dtext d = dtext{};
            auto ctx = "invalid"_ctx;
            while (!ctx.done())
            {
                assert(d(ctx));
            }
        }
    }

    // 无效值
    {
        constexpr auto domain_literal_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = domain_literal{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!domain_literal_rule("."_span));
    }

    return 0;
}
// NOLINTEND