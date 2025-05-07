#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // phrase          =   1*word / obs-phrase
    // 有效边界值
    {
        constexpr auto phrase_rule = [](const auto &span) constexpr {
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = phrase{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 标准短语
        static_assert(phrase_rule(R"("display name")"_span));
        static_assert(phrase_rule("john.doe"_span));
        static_assert(phrase_rule("first.last"_span));

        {
            auto obs = obs_phrase{};
            auto ctx = "."_ctx;
            auto ret = obs(ctx);
            assert(!ret);
            {
                auto ctx = "john.doe"_ctx;
                auto ret = obs(ctx);
                assert(ret);
                assert(ctx.cur_index == "john.doe"_span.size());
                assert(ctx.done());
            }
        }
        {
            auto wo = one_or_more<word>{};
            auto ctx = "john.doe"_ctx;
            auto ret = wo(ctx);
            assert(ret); // TODO 需要 chceck done 语法，来转移了
        }
    }

    // 无效值
    {
        constexpr auto phrase_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = phrase{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!phrase_rule("."_span));

        // 带点号短语
    }

    return 0;
}
// NOLINTEND