
#include "../test_abnf.hpp"
#include <optional>

// NOLINTBEGIN
using namespace mcs::abnf;

#include <iostream>

int main()
{
    TEST("ALPHA") = [] {
        static constexpr OCTET test_char = 'A';
        constexpr std::span<const OCTET> s(&test_char, 1);

        // 使用 static_assert 验证解析结果
        static_assert(ALPHA{}(make_parser_ctx(s)));
        static_assert(alpha(make_parser_ctx(s)).value() == 1);
        static_assert(*alpha(make_parser_ctx(s)) == 1);

        static_assert(alpha("A"_ctx)); // 大写字母
        static_assert(alpha("Zaaa"_ctx));
        static_assert(alpha("Zaaa"_ctx).has_value());
        static_assert(alpha("Zaaa"_ctx).value() == 1);
        static_assert(alpha("a"_ctx)); // 小写字母
        static_assert(alpha("n"_ctx));
        static_assert(alpha("z"_ctx));

        static_assert(!alpha(""_ctx)); // 非字母字符
        static_assert(!alpha("@"_ctx));
        static_assert(!alpha("["_ctx));
        static_assert(!alpha("\0"_ctx));
        static_assert(!alpha("\t"_ctx));
        static_assert(!alpha("\t"_ctx).has_value());

        static_assert(alpha("a["_ctx));
    };
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND