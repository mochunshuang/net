
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
        constexpr auto path_noscheme_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = path_noscheme{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 单个segment-nz-nc测试
        static constexpr octet single_segment[] = {'a', 'b', '@', 'c'};
        static_assert(path_noscheme_rule(single_segment));

        // 多段路径测试
        static constexpr octet multi_segment[] = {'a', '@', 'b', '/', 'c', '/', 'd'};
        static_assert(path_noscheme_rule(multi_segment));

        // 包含百分比编码测试
        static constexpr octet with_encoding[] = {'a', '%', '4', '1', '/', 'b'};
        static_assert(path_noscheme_rule(with_encoding));

        // 长路径测试
        static constexpr octet long_path[] = {'a', '!', '@', '1', '/', 'b', 'c',
                                              'd', '/', '%', '4', '1', '/', '(',
                                              ')', '/', '2', '3', '4'};
        static_assert(path_noscheme_rule(long_path));
    }

    // 无效测试用例
    {
        constexpr auto path_noscheme_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = path_noscheme{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };

        // 无效冒号字符测试
        static constexpr octet with_colon[] = {'a', ':', 'b'};
        static_assert(path_noscheme_rule(with_colon));
        static_assert(path_noscheme_rule(with_colon).value() == 1);

        // 空输入测试
        static_assert(!path_noscheme_rule(""_span));

        // 无效起始字符测试
        static constexpr octet invalid_start[] = {'/', 'a', 'b'};
        static_assert(!path_noscheme_rule(invalid_start));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
