
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
        constexpr auto query_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::query{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        // 空片段测试
        static_assert(query_rule(""_ctx));

        // 纯pchar字符测试
        static constexpr octet pchars_only[] = {'a', '1', '-', '_', '.', '~', '!', '&'};
        static_assert(query_rule(detail::make_parser_ctx(pchars_only)));

        // 包含斜杠测试
        static constexpr octet with_slash[] = {'a', '/', 'b', '/', 'c'};
        static_assert(query_rule(detail::make_parser_ctx(with_slash)));

        // 包含问号测试
        static constexpr octet with_question[] = {'q', '?', 'p', 'a', 'r',
                                                  'a', 'm', '=', '1'};
        static_assert(query_rule(detail::make_parser_ctx(with_question)));

        // 百分比编码测试
        static constexpr octet pct_encoded[] = {'%', '2', '0', '/', '%', '3', 'F', '?'};
        static_assert(query_rule(detail::make_parser_ctx(pct_encoded)));

        // 混合有效字符测试
        // 混合pchar、斜杠和问号测试
        static constexpr octet mixed_valid[] = {'s', 'e', 'a', 'r', 'c', 'h', '?',
                                                'q', '=', '1', '&', 'p', '/', '2'};
        static_assert(query_rule(detail::make_parser_ctx(mixed_valid)));

        // 边界情况测试
        static constexpr octet boundary1[] = {'/', '?', 'a'}; // 特殊字符组合
        static constexpr octet boundary2[] = {'?', '/', '%', '4',
                                              '1'}; // 混合特殊字符和编码
        static_assert(query_rule(detail::make_parser_ctx(boundary1)));
        static_assert(query_rule(detail::make_parser_ctx(boundary2)));

        // 长片段测试
        // 长查询字符串测试
        static constexpr octet long_query[] = {
            'a', '=', '1', '&', 'b', '=', '2', '&', 'c', '=', '3', '/', 'p',
            'a', 't', 'h', '?', 'q', 'u', 'e', 'r', 'y', '=', 'v', 'a', 'l'};
        static_assert(query_rule(detail::make_parser_ctx(long_query)));

        // 合法片段：包含编码后的# (%23)
        static constexpr octet encoded_hash[] = {'f', 'r', '%', '2', '3', 'a', 'g'};
        static_assert(query_rule(detail::make_parser_ctx(encoded_hash)));

        static constexpr octet incomplete_pct[] = {'a', '%', '2', 'b'};
        static_assert(query_rule(detail::make_parser_ctx(incomplete_pct)));
        static_assert(query_rule(detail::make_parser_ctx(incomplete_pct)).value() == 4);
    }
    {
        constexpr auto query_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::query{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        // 无效字符测试
        static constexpr octet invalid_chars1[] = {' '};
        static constexpr octet invalid_chars2[] = {'#'};
        static constexpr octet invalid_chars3[] = {'['};
        static_assert(query_rule(detail::make_parser_ctx(invalid_chars1)));
        static_assert(query_rule(detail::make_parser_ctx(invalid_chars1)).value() == 0);

        static_assert(query_rule(detail::make_parser_ctx(invalid_chars2)));
        static_assert(query_rule(detail::make_parser_ctx(invalid_chars2)).value() == 0);

        static_assert(query_rule(detail::make_parser_ctx(invalid_chars3)));
        static_assert(query_rule(detail::make_parser_ctx(invalid_chars3)).value() == 0);

        // 不完整的百分比编码测试
        static constexpr octet incomplete_pct[] = {'a', '%', '2'};
        {
            auto ctx = detail::make_parser_ctx(incomplete_pct);
            assert(ctx.cur_index == 0);
            assert(uri::query{}(ctx));
            assert(ctx.cur_index == 1);

            // NOTE: query = *( pchar / "/" / "?" )
            // NOTE: 因此就算ctx 不动，也是转移成功的
            // NOTE: 有返回值只能说明 转移成功。匹配当前规则。索引可能不变
            assert(uri::query{}(ctx));
            std::cout << "ctx.cur_index: " << ctx.cur_index << '\n';
            assert(ctx.cur_index == 1);
        }

        // 混合有效和无效字符测试
        static constexpr octet mixed_invalid[] = {'a', '/', ' ', '?', 'b'};
        {
            constexpr auto query_rule = [](parser_ctx ctx) constexpr {
                assert(ctx.cur_index == 0);
                auto suc = mcs::abnf::uri::query{}(ctx);
                return suc;
            };
            static_assert(query_rule(detail::make_parser_ctx(mixed_invalid)));
            static_assert(query_rule(detail::make_parser_ctx(mixed_invalid)).value() ==
                          2);
            static_assert(query_rule(detail::make_parser_ctx(mixed_invalid)).value() !=
                          5);
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
