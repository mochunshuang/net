
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
        constexpr auto fragment_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::fragment{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        // 空片段测试
        static_assert(fragment_rule(""_ctx));

        // 纯pchar字符测试
        static constexpr octet pchars_only[] = {'a', '1', '-', '_', '.', '~', '!', '&'};
        static_assert(fragment_rule(detail::make_parser_ctx(pchars_only)));

        // 包含斜杠测试
        static constexpr octet with_slash[] = {'a', '/', 'b', '/', 'c'};
        static_assert(fragment_rule(detail::make_parser_ctx(with_slash)));

        // 包含问号测试
        static constexpr octet with_question[] = {'q', '?', 'p', 'a', 'r',
                                                  'a', 'm', '=', '1'};
        static_assert(fragment_rule(detail::make_parser_ctx(with_question)));

        // 百分比编码测试
        static constexpr octet pct_encoded[] = {'%', '2', '0', '/', '%', '3', 'F', '?'};
        static_assert(fragment_rule(detail::make_parser_ctx(pct_encoded)));

        // 混合有效字符测试
        static constexpr octet mixed_valid[] = {'s', 'e', 'c', 't', 'i', 'o', 'n',
                                                '/', '1', '?', 'a', '=', 'b'};
        static_assert(fragment_rule(detail::make_parser_ctx(mixed_valid)));

        // 边界情况测试
        static constexpr octet boundary1[] = {'/', '?', 'a'}; // 特殊字符组合
        static constexpr octet boundary2[] = {'?', '/', '%', '4',
                                              '1'}; // 混合特殊字符和编码
        static_assert(fragment_rule(detail::make_parser_ctx(boundary1)));
        static_assert(fragment_rule(detail::make_parser_ctx(boundary2)));

        // 长片段测试
        static constexpr octet long_fragment[] = {'s', 'e', 'c', 't', 'i', 'o', 'n', '/',
                                                  '1', '/', '2', '?', 'p', 'a', 'r', 'a',
                                                  'm', '=', 'v', 'a', 'l', '%', '2', '0'};
        static_assert(fragment_rule(detail::make_parser_ctx(long_fragment)));

        // 合法片段：包含编码后的# (%23)
        static constexpr octet encoded_hash[] = {'f', 'r', '%', '2', '3', 'a', 'g'};
        static_assert(fragment_rule(detail::make_parser_ctx(encoded_hash)));
    }
    {
        constexpr auto fragment_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::IP_literal{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        // 无效字符测试
        static constexpr octet invalid_chars1[] = {' '};
        static constexpr octet invalid_chars2[] = {'#'};
        static constexpr octet invalid_chars3[] = {'['};
        static_assert(!fragment_rule(detail::make_parser_ctx(invalid_chars1)));
        static_assert(!fragment_rule(detail::make_parser_ctx(invalid_chars2)));
        static_assert(!fragment_rule(detail::make_parser_ctx(invalid_chars3)));

        // 不完整的百分比编码测试
        static constexpr octet incomplete_pct1[] = {'a', '%', '2', 'b'};
        static constexpr octet incomplete_pct2[] = {'%', 'A'};
        static_assert(!fragment_rule(detail::make_parser_ctx(incomplete_pct1)));
        static_assert(!fragment_rule(detail::make_parser_ctx(incomplete_pct2)));

        // 无效百分比编码测试
        static constexpr octet invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!fragment_rule(detail::make_parser_ctx(invalid_pct)));

        // 非法片段：直接包含#
        static constexpr octet raw_hash[] = {'f', 'r', '#', 'a', 'g'};
        static_assert(!fragment_rule(detail::make_parser_ctx(raw_hash)));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
