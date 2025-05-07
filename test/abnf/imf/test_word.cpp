#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto word_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = word{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // atom有效测试
        static_assert(word_rule("simple"_span));
        static_assert(word_rule("atom!#$%&'*+"_span));

        // quoted_string有效测试
        static_assert(word_rule(R"("quoted string")"_span));
        static_assert(word_rule(R"("escaped\"quote")"_span));
    }

    // 无效值
    {
        constexpr auto word_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = word{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(word_rule("has space"_span));
        static_assert(!word_rule(R"("unclosed)"_span));
    }

    return 0;
}
// NOLINTEND