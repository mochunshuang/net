
#include "../test_head.hpp"

// NOLINTBEGIN

#include <iostream>

int main()
{
    {
        { // 有效百分比编码
            static_assert(mcs::abnf::uri::pct_encoded('%', 'A', 'F'));
            static_assert(mcs::abnf::uri::pct_encoded('%', 'a', 'f'));
            static_assert(mcs::abnf::uri::pct_encoded('%', '0', '9'));
            static_assert(mcs::abnf::uri::pct_encoded('%', '9', 'A'));
            static_assert(mcs::abnf::uri::pct_encoded('%', 'F', '0'));
        }

        { // 无效格式 - 缺少%符号
            static_assert(!mcs::abnf::uri::pct_encoded('A', 'F', '1'));
            static_assert(!mcs::abnf::uri::pct_encoded(' ', 'A', 'B'));
            static_assert(!mcs::abnf::uri::pct_encoded('#', '1', '2'));
        }

        { // 无效格式 - 非HEXDIG字符
            static_assert(!mcs::abnf::uri::pct_encoded('%', 'G', '0'));
            static_assert(!mcs::abnf::uri::pct_encoded('%', ' ', 'A'));
            static_assert(!mcs::abnf::uri::pct_encoded('%', '@', '1'));
            static_assert(!mcs::abnf::uri::pct_encoded('%', '0', 'g'));
            static_assert(!mcs::abnf::uri::pct_encoded('%', 'A', ' '));
        }

        {                                                              // 边界情况测试
            static_assert(mcs::abnf::uri::pct_encoded('%', '0', '0')); // 最小值
            static_assert(mcs::abnf::uri::pct_encoded('%', 'F', 'F')); // 最大值
            static_assert(!mcs::abnf::uri::pct_encoded('%', '0' - 1, '0')); // 低于最小值
            static_assert(!mcs::abnf::uri::pct_encoded('%', 'F', 'F' + 1)); // 高于最大值
        }

        { // 大小写敏感测试
            static_assert(mcs::abnf::uri::pct_encoded('%', 'A', 'a'));
            static_assert(mcs::abnf::uri::pct_encoded('%', 'f', 'F'));
        }
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND