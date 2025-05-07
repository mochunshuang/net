#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

using namespace mcs::abnf;
using namespace mcs::abnf::imf;

int main()
{

    // 有效边界值
    {
        constexpr auto quoted_string_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = quoted_string{}(ctx);
            assert(ctx.cur_index == span.size());
            assert(ctx.done());
            return suc;
        };
        // 标准有效测试
        constexpr auto valid1 = R"("simple")"_span;
        static_assert(quoted_string_rule(valid1));

        // 带转义字符
        constexpr auto valid2 = R"("quote\"here")"_span;
        static_assert(quoted_string_rule(valid2));

        // 带CFWS
        constexpr auto valid3 = " (comment)\" valid string \""_span;
        static_assert(quoted_string_rule(valid3));
    }

    // 无效值
    {
        constexpr auto quoted_string_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = quoted_string{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            assert(not ctx.done());
            return suc;
        };
        static_assert(!quoted_string_rule("(\"invalid\a\")"_span)); // 无效转义
        static_assert(!quoted_string_rule(R"(unquoted)"_span));
    }

    return 0;
}
// NOLINTEND