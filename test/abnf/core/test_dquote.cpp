
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("dquote") = [] {
        static_assert(not dquote("1"_ctx));
        static_assert(!dquote("1aaa"_ctx));

        static_assert(!dquote("0"_ctx));

        // 无效字符测试
        static_assert(not dquote("a"_ctx));

        // 空输入测试
        static_assert(not dquote(""_ctx));

        static_assert(dquote("\""_ctx));
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND