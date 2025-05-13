
#include "../test_abnf.hpp"
#include "./test_uri.hpp"
#include <cassert>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;

#include <iostream>

int main()
{

    // 有效测试用例
    {
        constexpr auto pchar_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::pchar{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        constexpr auto pchar_rule_all = [=](std::span<const octet> span) constexpr {
            bool ret = true;
            while (!span.empty())
            {
                auto check_span = span.first(1);
                ret = ret && pchar_rule(make_parser_ctx(check_span));
                span = span.subspan(1);
            }
            return ret;
        };
        // 空输入测试
        static_assert(not pchar_rule(""_ctx));

        // 纯unreserved字符测试
        static constexpr octet unreserved_chars[] = {'a', 'Z', '0', '-', '.', '_', '~'};
        static_assert(pchar_rule_all(unreserved_chars));

        // 纯sub-delims字符测试
        static constexpr octet subdelims_chars[] = {'!', '$', '&', '\'', '(', ')',
                                                    '*', '+', ',', ';',  '='};
        static_assert(pchar_rule_all(subdelims_chars));

        // 混合字符测试
        static constexpr octet mixed_chars[] = {'a', '!', '1', '$', '.', '(', '~'};
        static_assert(pchar_rule_all(mixed_chars));

        // 百分比编码测试
        static constexpr octet pct_encoded[] = {'%', '4', '1', '%', 'A', 'F', 'a'};
        {
            auto ctx = make_parser_ctx(pct_encoded);
            assert(pchar{}(ctx));
            assert(ctx.cur_index == 3);
            std::cout << "ctx.cur_index: " << ctx.cur_index << '\n';
            assert(pchar{}(ctx));
            std::cout << "ctx.cur_index: " << ctx.cur_index << '\n';
            assert(ctx.cur_index == 6);
            assert(pchar{}(ctx));
            assert(ctx.cur_index == 7);
        }

        // 边界情况测试
        static constexpr octet boundary1[] = {':', '@'}; // 特殊允许字符
        static_assert(pchar_rule_all(boundary1));
    }
    {
        constexpr auto pchar_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::pchar{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        // 无效字符测试
        static constexpr octet invalid_chars1[] = {' '};
        static constexpr octet invalid_chars2[] = {'"'};
        static constexpr octet invalid_chars3[] = {'<'};
        static_assert(!pchar_rule(make_parser_ctx(invalid_chars1)));
        static_assert(!pchar_rule(make_parser_ctx(invalid_chars2)));
        static_assert(!pchar_rule(make_parser_ctx(invalid_chars3)));

        // 不完整的百分比编码测试
        static constexpr octet incomplete_pct1[] = {'%'};
        static constexpr octet incomplete_pct2[] = {'%', 'A'};
        static_assert(!pchar_rule(make_parser_ctx(incomplete_pct1)));
        static_assert(!pchar_rule(make_parser_ctx(incomplete_pct2)));

        // 无效百分比编码测试
        static constexpr octet invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!pchar_rule(make_parser_ctx(invalid_pct)));

        // 混合有效和无效字符测试
        static constexpr octet mixed_invalid[] = {'a', '!', ' ', '1'};
        {
            auto ctx = make_parser_ctx(mixed_invalid);
            assert(pchar{}(ctx));
            assert(ctx.cur_index == 1);
            assert(pchar{}(ctx));
            assert(ctx.cur_index == 2);

            assert(not pchar{}(ctx)); // ' '
            assert(ctx.cur_index == 2);
        }

        // 边界情况测试
        static constexpr octet boundary2[] = {'#', '?'}; // gen-delims应该无效
        static_assert(!pchar_rule(make_parser_ctx(boundary2)));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
