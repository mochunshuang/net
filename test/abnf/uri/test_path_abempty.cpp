
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
        constexpr auto path_abempty_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = path_abempty{}(ctx);
            assert(ctx.cur_index == span.size());
            return suc;
        };
        // 空路径测试
        static_assert(path_abempty_rule(""_span));

        // 仅斜杠测试
        static constexpr OCTET single_slash[] = {'/'};
        static_assert(path_abempty_rule(single_slash));

        // 多斜杠测试
        static constexpr OCTET multiple_slashes[] = {'/', '/', '/'};
        static_assert(path_abempty_rule(multiple_slashes));

        // 有效segment测试
        static constexpr OCTET valid_segments[] = {'/', 'a', '/', 'b', '/', 'c'};
        static_assert(path_abempty_rule(valid_segments));

        // 混合斜杠和segment测试
        static constexpr OCTET mixed_valid[] = {'/', 'a', 'b', '/', '/',
                                                'c', 'd', '/', 'e'};
        static_assert(path_abempty_rule(mixed_valid));

        // 百分比编码segment测试
        static constexpr OCTET pct_encoded[] = {'/', '%', '4', '1', '/', '%', 'A', 'F'};
        static_assert(path_abempty_rule(pct_encoded));

        // 长路径测试
        static constexpr OCTET long_path[] = {'/', 'a', 'b', 'c', '/', '1', '2', '3', '/',
                                              '!', '$', '&', '/', '%', '4', '1', '/', 'x',
                                              'y', 'z', '/', '/', 'e', 'n', 'd'};
        static_assert(path_abempty_rule(long_path));

        // 边界情况测试
        static constexpr OCTET boundary1[] = {'/', 'a'};           // 单segment
        static constexpr OCTET boundary2[] = {'/', 'a', '/', '/'}; // segment后多斜杠
        static_assert(path_abempty_rule(boundary1));
        static_assert(path_abempty_rule(boundary2));
    }

    // 无效测试用例
    {
        constexpr auto path_abempty_rule = [](const auto &array) constexpr {
            auto span = std::span{array};
            parser_ctx ctx = make_parser_ctx(span);
            assert(ctx.cur_index == 0);
            auto suc = path_abempty{}(ctx);
            assert(ctx.cur_index != span.size() || ctx.cur_index == 0);
            return suc;
        };

        // 无效起始字符测试
        static constexpr OCTET invalid_start[] = {'a', '/', 'b'};
        static_assert(path_abempty_rule(invalid_start));
        static_assert(*path_abempty_rule(invalid_start) == 0);

        // 包含无效字符的segment测试
        static constexpr OCTET invalid_segment[] = {'/', 'a', ' ', '/', 'b'};
        static_assert(path_abempty_rule(invalid_segment));
        static_assert(*path_abempty_rule(invalid_segment) == 2);

        // 不完整的百分比编码测试
        static constexpr OCTET incomplete_pct[] = {'/', 'a', '%', '2', '/', 'b'};
        static_assert(path_abempty_rule(incomplete_pct));
        static_assert(*path_abempty_rule(incomplete_pct) == 2);
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
