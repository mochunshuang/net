
#include "../test_abnf.hpp"
#include "./test_uri.hpp"

// NOLINTBEGIN
using namespace mcs::abnf;
using namespace mcs::abnf::uri;
#include <array>
#include <iostream>

int main()
{
    constexpr IPv4address ipv4address{};

    { // 最小长度IPv4 (1.1.1.1)
        static constexpr octet min_ip[] = {'1', '.', '1', '.', '1', '.', '1'};
        static_assert(ipv4address(make_parser_ctx(min_ip)));
    }

    { // 标准IPv4 (192.168.1.1)
        static constexpr octet standard_ip[] = {'1', '9', '2', '.', '1', '6',
                                                '8', '.', '1', '.', '1'};
        static_assert(ipv4address(make_parser_ctx(standard_ip)));
    }

    { // 边界值IPv4 (0.0.0.0 和 255.255.255.255)
        static constexpr octet min_bound[] = {'0', '.', '0', '.', '0', '.', '0'};
        static constexpr octet max_bound[] = {'2', '5', '5', '.', '2', '5', '5', '.',
                                              '2', '5', '5', '.', '2', '5', '5'};
        static_assert(ipv4address(make_parser_ctx(min_bound)));
        static_assert(ipv4address(make_parser_ctx(max_bound)));
        static_assert(ipv4address(make_parser_ctx(max_bound)));
    }

    { // 无效格式 - 不足最小长度
        static constexpr octet too_short[] = {'1', '.', '1', '.', '1'};
        static_assert(!ipv4address(make_parser_ctx(too_short)));
    }

    { // 无效格式 - 点号位置错误
        static constexpr octet wrong_dots1[] = {'.', '1', '.', '1', '.', '1', '.', '1'};
        static constexpr octet wrong_dots2[] = {'1', '.', '1', '.', '1', '.', '1', '.'};
        static_assert(!ipv4address(make_parser_ctx(wrong_dots1)));
        // NOTE: 最后的点，被截断了
        static_assert(ipv4address(make_parser_ctx(wrong_dots2)));
    }

    { // 无效格式 - 包含非数字字符
        static constexpr octet non_digit[] = {'1', '2', '7', '.', '0',
                                              '.', '0', '.', 'a'};
        static_assert(!ipv4address(make_parser_ctx(non_digit)));
    }

    { // 无效格式 - 八位组超出范围
        static constexpr octet overflow1[] = {'2', '5', '6', '.', '0',
                                              '.', '0', '.', '1'};
        static constexpr octet overflow2[] = {'1', '2', '8', '.', '2', '5',
                                              '6', '.', '1', '.', '1'};
        static_assert(!ipv4address(make_parser_ctx(overflow1)));
        static_assert(!ipv4address(make_parser_ctx(overflow2)));
    }

    { // 有效混合长度IPv4 (10.0.100.1)
        static constexpr octet mixed_len[] = {'1', '0', '.', '0', '.',
                                              '1', '0', '0', '.', '1'};
        static_assert(ipv4address(make_parser_ctx(mixed_len)));
    }

    { // 无效格式 - 连续点号
        static constexpr octet consecutive_dots[] = {'1', '0', '.', '.', '0',
                                                     '.', '1', '.', '1'};
        static_assert(!ipv4address(make_parser_ctx(consecutive_dots)));
    }

    { // 无效格式 - 前导零 (根据RFC应视为无效，但取决于具体实现)
        static constexpr octet leading_zero[] = {'0', '1', '0', '.', '0', '0', '1',
                                                 '.', '0', '1', '.', '0', '0', '1'};
        static_assert(!ipv4address(make_parser_ctx(leading_zero)));
    }

    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
