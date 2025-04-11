
#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

#include <array>
#include <iostream>

int main()
{
    { // 测试单个数字 0-9
        static constexpr mcs::ABNF::OCTET valid1[] = {'0'};
        static constexpr mcs::ABNF::OCTET valid2[] = {'5'};
        static constexpr mcs::ABNF::OCTET valid3[] = {'9'};
        static_assert(mcs::ABNF::URI::dec_octet(valid1).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(valid2).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(valid3).has_value());
    }

    { // 测试两位数 10-99
        static constexpr mcs::ABNF::OCTET valid1[] = {'1', '0'};
        static constexpr mcs::ABNF::OCTET valid2[] = {'9', '9'};
        static constexpr mcs::ABNF::OCTET invalid1[] = {'0', '0'}; // 前导0无效
        static_assert(mcs::ABNF::URI::dec_octet(valid1).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(valid2).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(invalid1).has_value());
    }

    { // 测试100-199
        static constexpr mcs::ABNF::OCTET valid1[] = {'1', '0', '0'};
        static constexpr mcs::ABNF::OCTET valid2[] = {'1', '9', '9'};
        static constexpr mcs::ABNF::OCTET invalid1[] = {'1', 'a', '0'}; // 非数字
        static_assert(mcs::ABNF::URI::dec_octet(valid1).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(valid2).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(invalid1).has_value());
    }

    { // 测试200-255
        static constexpr mcs::ABNF::OCTET valid1[] = {'2', '0', '0'};
        static constexpr mcs::ABNF::OCTET valid2[] = {'2', '5', '5'};
        static constexpr mcs::ABNF::OCTET invalid1[] = {'2', '5', '6'}; // 应走25x分支
        static constexpr mcs::ABNF::OCTET invalid2[] = {'2', 'a', '0'}; // 非数字
        static_assert(mcs::ABNF::URI::dec_octet(valid1).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(valid2).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(invalid1).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(invalid2).has_value());
    }

    { // 测试250-255
        static constexpr mcs::ABNF::OCTET valid1[] = {'2', '5', '0'};
        static constexpr mcs::ABNF::OCTET valid2[] = {'2', '5', '5'};
        static constexpr mcs::ABNF::OCTET invalid1[] = {'2', '5', '6'}; // 超过255
        static constexpr mcs::ABNF::OCTET invalid2[] = {'2', '4', '5'}; // 应走24x分支
        static_assert(mcs::ABNF::URI::dec_octet(valid1).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(valid2).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(invalid1).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(invalid2).has_value()); // 245是有效值
    }

    { // 测试无效长度
        constexpr std::array<mcs::ABNF::OCTET, 0> invalid0{};
        static constexpr mcs::ABNF::OCTET invalid4[] = {'1', '2', '3', '4'};
        static_assert(!mcs::ABNF::URI::dec_octet(invalid0).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(invalid4).has_value());
    }

    { // 测试边界值
        static constexpr mcs::ABNF::OCTET max[] = {'2', '5', '5'};
        static constexpr mcs::ABNF::OCTET min[] = {'0'};
        static constexpr mcs::ABNF::OCTET overflow[] = {'2', '5', '6'};
        static_assert(mcs::ABNF::URI::dec_octet(max).has_value());
        static_assert(mcs::ABNF::URI::dec_octet(min).has_value());
        static_assert(!mcs::ABNF::URI::dec_octet(overflow).has_value());
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
