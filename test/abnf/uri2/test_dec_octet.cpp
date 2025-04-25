
#include "../test_abnf.hpp"
#include "./test_uri.hpp"

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;
#include <array>
#include <iostream>

int main()
{
    constexpr dec_octet dec_octet_ruler{};
    {                                            // 测试单个数字 0-9
        static constexpr octet valid1[] = {'0'}; // NOLINT
        static constexpr octet valid2[] = {'5'}; // NOLINT
        static constexpr octet valid3[] = {'9'}; // NOLINT
        static_assert(dec_octet_ruler(make_parser_ctx(valid1)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid2)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid3)));
    }

    { // 测试两位数 10-99
        static constexpr octet valid1[] = {'1', '0'};
        static constexpr octet valid2[] = {'9', '9'};
        static constexpr octet invalid1[] = {'0', '0'}; // 前导0无效
        static_assert(dec_octet_ruler(make_parser_ctx(valid1)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid1)).value() == 2);
        static_assert(dec_octet_ruler(make_parser_ctx(valid2)));
        static_assert(dec_octet_ruler(make_parser_ctx(invalid1)));
    }

    { // 测试100-199
        static constexpr octet valid1[] = {'1', '0', '0'};
        static constexpr octet valid2[] = {'1', '9', '9'};
        static constexpr octet invalid1[] = {'1', 'a', '0'}; // 非数字
        static_assert(dec_octet_ruler(make_parser_ctx(valid1)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid2)));
        static_assert(dec_octet_ruler(make_parser_ctx(invalid1)));
    }

    { // 测试200-255
        static constexpr octet valid1[] = {'2', '0', '0'};
        static constexpr octet valid2[] = {'2', '5', '5'};
        static constexpr octet valid3[] = {'2', '5', '6'}; // 应走25x分支
        static constexpr octet valid4[] = {'2', 'a', '0'}; // 非数字
        static_assert(dec_octet_ruler(make_parser_ctx(valid1)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid2)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid3)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid4)));
    }

    { // 测试250-255
        static constexpr octet valid1[] = {'2', '5', '0'};
        static constexpr octet valid2[] = {'2', '5', '5'};
        static constexpr octet valid3[] = {'2', '5', '6'}; // 超过255
        static constexpr octet valid4[] = {'2', '4', '5'}; // 应走24x分支
        static_assert(dec_octet_ruler(make_parser_ctx(valid1)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid2)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid3)));
        static_assert(dec_octet_ruler(make_parser_ctx(valid4)));
    }

    { // 测试无效长度
        constexpr std::array<octet, 0> invalid0{};
        static constexpr octet invalid4[] = {'1', '2', '3', '4'};
        static_assert(not dec_octet_ruler(make_parser_ctx(invalid0)));
        static_assert(dec_octet_ruler(make_parser_ctx(invalid4)));
    }

    { // 测试边界值
        static constexpr octet max[] = {'2', '5', '5'};
        static constexpr octet min[] = {'0'};
        static constexpr octet overflow[] = {'2', '5', '6'};
        static_assert(dec_octet_ruler(make_parser_ctx(max)));
        static_assert(dec_octet_ruler(make_parser_ctx(min)));
        {
            // NOTE: 仅仅从返回值看，是看不出是否正确的. 还要和大小匹配. 3
            // 才算成功，但是不知道怎么描述 NOTE: 2 确实是满足的
            static_assert(dec_octet_ruler(make_parser_ctx(overflow)));
            static_assert(dec_octet_ruler(make_parser_ctx(overflow)).value() == 2);
        }
    }
    { // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
        using IPv4address =
            sequence<dec_octet, InsensitiveChar<'.'>, dec_octet, InsensitiveChar<'.'>,
                     dec_octet, InsensitiveChar<'.'>, dec_octet>;
        { // 最小长度IPv4 (1.1.1.1)
            static constexpr octet min_ip[] = {'1', '.', '1', '.', '1', '.', '1'};
            static_assert(IPv4address{}(make_parser_ctx(min_ip)));
            static_assert(IPv4address{}(make_parser_ctx(min_ip)).value() == 7);
        }
        { // 1.1.1.
            static constexpr octet min_ip[] = {'1', '.', '1', '.', '1', '.'};
            static_assert(not IPv4address{}(make_parser_ctx(min_ip)));
        }
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
