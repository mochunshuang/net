
#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

using OCTET = mcs::ABNF::OCTET;

int main()
{
    {
        { // 最小长度IPv4 (1.1.1.1)
            static constexpr mcs::ABNF::OCTET min_ip[] = {'1', '.', '1', '.',
                                                          '1', '.', '1'};
            static_assert(mcs::ABNF::URI::IPv4address(min_ip));
        }

        { // 标准IPv4 (192.168.1.1)
            static constexpr mcs::ABNF::OCTET standard_ip[] = {
                '1', '9', '2', '.', '1', '6', '8', '.', '1', '.', '1'};
            static_assert(mcs::ABNF::URI::IPv4address(standard_ip));
        }

        { // 边界值IPv4 (0.0.0.0 和 255.255.255.255)
            static constexpr mcs::ABNF::OCTET min_bound[] = {'0', '.', '0', '.',
                                                             '0', '.', '0'};
            static constexpr mcs::ABNF::OCTET max_bound[] = {'2', '5', '5', '.', '2',
                                                             '5', '5', '.', '2', '5',
                                                             '5', '.', '2', '5', '5'};
            static_assert(mcs::ABNF::URI::IPv4address(min_bound));
            static_assert(mcs::ABNF::URI::IPv4address(max_bound));
        }

        { // 无效格式 - 不足最小长度
            static constexpr mcs::ABNF::OCTET too_short[] = {'1', '.', '1', '.', '1'};
            static_assert(!mcs::ABNF::URI::IPv4address(too_short));
        }

        { // 无效格式 - 点号位置错误
            static constexpr mcs::ABNF::OCTET wrong_dots1[] = {'.', '1', '.', '1',
                                                               '.', '1', '.', '1'};
            static constexpr mcs::ABNF::OCTET wrong_dots2[] = {'1', '.', '1', '.',
                                                               '1', '.', '1', '.'};
            static_assert(!mcs::ABNF::URI::IPv4address(wrong_dots1));
            static_assert(!mcs::ABNF::URI::IPv4address(wrong_dots2));
        }

        { // 无效格式 - 包含非数字字符
            static constexpr mcs::ABNF::OCTET non_digit[] = {'1', '2', '7', '.', '0',
                                                             '.', '0', '.', 'a'};
            static_assert(!mcs::ABNF::URI::IPv4address(non_digit));
        }

        { // 无效格式 - 八位组超出范围
            static constexpr mcs::ABNF::OCTET overflow1[] = {'2', '5', '6', '.', '0',
                                                             '.', '0', '.', '1'};
            static constexpr mcs::ABNF::OCTET overflow2[] = {'1', '2', '8', '.', '2', '5',
                                                             '6', '.', '1', '.', '1'};
            static_assert(!mcs::ABNF::URI::IPv4address(overflow1));
            static_assert(!mcs::ABNF::URI::IPv4address(overflow2));
        }

        { // 有效混合长度IPv4 (10.0.100.1)
            static constexpr mcs::ABNF::OCTET mixed_len[] = {'1', '0', '.', '0', '.',
                                                             '1', '0', '0', '.', '1'};
            static_assert(mcs::ABNF::URI::IPv4address(mixed_len));
        }

        { // 无效格式 - 连续点号
            static constexpr mcs::ABNF::OCTET consecutive_dots[] = {
                '1', '0', '.', '.', '0', '.', '1', '.', '1'};
            static_assert(!mcs::ABNF::URI::IPv4address(consecutive_dots));
        }

        { // 无效格式 - 前导零 (根据RFC应视为无效，但取决于具体实现)
            static constexpr mcs::ABNF::OCTET leading_zero[] = {
                '0', '1', '0', '.', '0', '0', '1', '.', '0', '1', '.', '0', '0', '1'};
            static_assert(!mcs::ABNF::URI::IPv4address(leading_zero));
        }
    }
    return 0;
}

// NOLINTEND