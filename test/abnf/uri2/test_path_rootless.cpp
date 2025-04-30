
#include "../test_abnf.hpp"
#include "./test_uri.hpp"
#include <cassert>
#include <array>

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;

#include <iostream>

int main()
{

    // 有效测试用例
    {
        constexpr auto path_rootless_rule = [](auto span) constexpr {
            parser_ctx ctx = make_parser_ctx(span);
            auto suc = mcs::abnf::uri::path_rootless{}(ctx);
            assert(ctx.empty());
            return suc;
        };
        constexpr std::array<uint8_t, 1> input{'a'};
        static_assert(path_rootless_rule(input));
        {
            constexpr std::array<uint8_t, 3> input{'a', '/', 'b'};
            static_assert(path_rootless_rule(input));
        }
        {
            constexpr std::array<uint8_t, 4> input{'a', '/', '/', 'b'};
            static_assert(path_rootless_rule(input));
        }
        {
            // : 是可以的
            constexpr std::array<uint8_t, 3> input{':', 'a', 'b'};
            static_assert(path_rootless_rule(input));
        }
    }

    // 无效测试用例
    {
        constexpr auto path_rootless_rule = [](auto span) constexpr {
            parser_ctx ctx = make_parser_ctx(span);
            auto suc = mcs::abnf::uri::path_rootless{}(ctx);
            assert(ctx.cur_index == 0);
            return suc;
        };
        {
            constexpr std::array<uint8_t, 0> input{};
            static_assert(!path_rootless_rule(input));
        }
        {
            constexpr std::array<uint8_t, 2> input{'/', 'a'};
            static_assert(!path_rootless_rule(input));
        }
        {
            constexpr auto path_rootless_rule = [](auto span) constexpr {
                parser_ctx ctx = make_parser_ctx(span);
                auto suc = mcs::abnf::uri::path_rootless{}(ctx);
                assert(ctx.cur_index != span.size());
                return suc;
            };
            // 假设空格是非法的pchar
            constexpr std::array<uint8_t, 5> input{'a', '/', 'b', ' ', 'c'};
            static_assert(path_rootless_rule(input));
            static_assert(path_rootless_rule(input).value() == 3);
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
