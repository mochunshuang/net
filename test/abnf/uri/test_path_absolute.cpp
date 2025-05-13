
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
        constexpr auto path_absolute_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = path_absolute{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 仅斜杠测试
        static constexpr octet slash_only[] = {'/'};
        static_assert(path_absolute_rule(slash_only));

        // 斜杠+segment-nz测试
        static constexpr octet with_segment[] = {'/', 'a', 'b', 'c'};
        static_assert(path_absolute_rule(with_segment));

        // 多段路径测试
        static constexpr octet multi_segment[] = {'/', 'a', '/', 'b', '/', 'c'};
        static_assert(path_absolute_rule(multi_segment));

        // 包含百分比编码测试
        static constexpr octet with_encoding[] = {'/', '%', '4', '1', '/', 'b'};
        static_assert(path_absolute_rule(with_encoding));

        // 长路径测试
        static constexpr octet long_path[] = {'/', 'a', 'b', '/', 'c', 'd',
                                              'e', '/', '%', '4', '1', '/',
                                              '!', '@', '/', '1', '2', '3'};
        static_assert(path_absolute_rule(long_path));
    }

    // 无效测试用例
    {
        constexpr auto path_absolute_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = mcs::abnf::uri::path_absolute{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };
        // 空路径测试
        static_assert(!path_absolute_rule(""_span));

        // 无效起始字符测试
        static constexpr octet invalid_start[] = {'a', '/', 'b'};
        static_assert(!path_absolute_rule(invalid_start));

        // 空segment-nz测试
        static constexpr octet empty_segment[] = {'/', '/', 'a'};
        static_assert(path_absolute_rule(empty_segment));
        static_assert(path_absolute_rule(empty_segment).value() == 1);
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
