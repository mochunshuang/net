
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("BIT") = [] {
        // 有效BIT测试
        static_assert(bit("1"_ctx));
        static_assert(bit("1aaa"_ctx));

        static_assert(bit("0"_ctx));

        // 无效字符测试
        static_assert(not bit("a"_ctx));

        // 空输入测试
        static_assert(not bit(""_ctx));
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND