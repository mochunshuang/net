
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
        constexpr auto pct_encoded_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::pct_encoded{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        // 有效百分比编码
        static constexpr octet valid_chars1[] = {'%', 'A', 'F'};
        static constexpr octet valid_chars2[] = {'%', 'a', 'f'};
        static constexpr octet valid_chars3[] = {'%', '9', 'A'};
        static constexpr octet valid_chars4[] = {'%', 'F', '0'};
        static_assert(pct_encoded_rule(make_parser_ctx(valid_chars1)));
        static_assert(pct_encoded_rule(make_parser_ctx(valid_chars2)));
        static_assert(pct_encoded_rule(make_parser_ctx(valid_chars3)));
        static_assert(pct_encoded_rule(make_parser_ctx(valid_chars4)));

        // 边界情况测试
        static constexpr octet boundary0[] = {'%', '0', '0'}; // 最小值
        static constexpr octet boundary1[] = {'%', 'F', 'F'}; // 最大值
        static_assert(pct_encoded_rule(make_parser_ctx(boundary0)));
        static_assert(pct_encoded_rule(make_parser_ctx(boundary1)));

        // 大小写敏感测试
        static constexpr octet valid0[] = {'%', 'A', 'a'};
        static constexpr octet valid1[] = {'%', 'f', 'F'};
        static_assert(pct_encoded_rule(make_parser_ctx(valid0)));
        static_assert(pct_encoded_rule(make_parser_ctx(valid1)));
    }
    {
        constexpr auto pct_encoded_rule = [](parser_ctx ctx) constexpr {
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::pct_encoded{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        // 空片段测试
        static_assert(!pct_encoded_rule(""_ctx));

        // 无效格式 - 缺少%符号
        static constexpr octet invalid0[] = {'A', 'F', '1'};
        static constexpr octet invalid1[] = {' ', 'A', 'B'};
        static constexpr octet invalid2[] = {'#', '1', '2'};
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid0)));
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid1)));
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid2)));

        // 无效格式 - 非HEXDIG字符
        static constexpr octet invalid_hexdig0[] = {'%', 'G', '0'};
        static constexpr octet invalid_hexdig1[] = {'%', ' ', 'A'};
        static constexpr octet invalid_hexdig2[] = {'%', '@', '1'};
        static constexpr octet invalid_hexdig3[] = {'%', '0', 'g'};
        static constexpr octet invalid_hexdig4[] = {'%', 'A', ' '};
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid_hexdig0)));
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid_hexdig1)));
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid_hexdig2)));
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid_hexdig3)));
        static_assert(!pct_encoded_rule(make_parser_ctx(invalid_hexdig4)));

        // 边界情况测试
        static constexpr octet boundary0[] = {'%', '0' - 1, '0'}; // 低于最小值
        static constexpr octet boundary1[] = {'%', 'F', 'F' + 1}; // 高于最大值
        static_assert(!pct_encoded_rule(make_parser_ctx(boundary0)));
        static_assert(!pct_encoded_rule(make_parser_ctx(boundary1)));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
