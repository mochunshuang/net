
#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

#include <cassert>
#include <iostream>

using namespace mcs::ABNF;

int main()
{
    { // 空片段测试
        static_assert(mcs::ABNF::URI::fragment(empty_span).has_value());
    }

    { // 纯pchar字符测试
        static constexpr mcs::ABNF::OCTET pchars_only[] = {'a', '1', '-', '_',
                                                           '.', '~', '!', '&'};
        static_assert(mcs::ABNF::URI::fragment(pchars_only).has_value());
    }

    { // 包含斜杠测试
        static constexpr mcs::ABNF::OCTET with_slash[] = {'a', '/', 'b', '/', 'c'};
        static_assert(mcs::ABNF::URI::fragment(with_slash).has_value());
    }

    { // 包含问号测试
        static constexpr mcs::ABNF::OCTET with_question[] = {'q', '?', 'p', 'a', 'r',
                                                             'a', 'm', '=', '1'};
        static_assert(mcs::ABNF::URI::fragment(with_question).has_value());
    }

    { // 百分比编码测试
        static constexpr mcs::ABNF::OCTET pct_encoded[] = {'%', '2', '0', '/',
                                                           '%', '3', 'F', '?'};
        static_assert(mcs::ABNF::URI::fragment(pct_encoded).has_value());
    }

    { // 混合有效字符测试
        static constexpr mcs::ABNF::OCTET mixed_valid[] = {
            's', 'e', 'c', 't', 'i', 'o', 'n', '/', '1', '?', 'a', '=', 'b'};
        static_assert(mcs::ABNF::URI::fragment(mixed_valid).has_value());
    }

    { // 无效字符测试
        static constexpr mcs::ABNF::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::ABNF::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::ABNF::OCTET invalid_chars3[] = {'['};
        static_assert(!mcs::ABNF::URI::fragment(invalid_chars1).has_value());
        static_assert(!mcs::ABNF::URI::fragment(invalid_chars2).has_value());
        static_assert(!mcs::ABNF::URI::fragment(invalid_chars3).has_value());
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::ABNF::OCTET incomplete_pct1[] = {'a', '%', '2', 'b'};
        static constexpr mcs::ABNF::OCTET incomplete_pct2[] = {'%', 'A'};
        static_assert(mcs::ABNF::URI::fragment(incomplete_pct1).has_value());
        static_assert(!mcs::ABNF::URI::fragment(incomplete_pct2).has_value());
    }

    { // 无效百分比编码测试
        static constexpr mcs::ABNF::OCTET invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!mcs::ABNF::URI::fragment(invalid_pct).has_value());
    }

    {                                                                    // 边界情况测试
        static constexpr mcs::ABNF::OCTET boundary1[] = {'/', '?', 'a'}; // 特殊字符组合
        static constexpr mcs::ABNF::OCTET boundary2[] = {'?', '/', '%', '4',
                                                         '1'}; // 混合特殊字符和编码
        static_assert(mcs::ABNF::URI::fragment(boundary1).has_value());
        static_assert(mcs::ABNF::URI::fragment(boundary2).has_value());
    }

    { // 长片段测试
        static constexpr mcs::ABNF::OCTET long_fragment[] = {
            's', 'e', 'c', 't', 'i', 'o', 'n', '/', '1', '/', '2', '?',
            'p', 'a', 'r', 'a', 'm', '=', 'v', 'a', 'l', '%', '2', '0'};
        static_assert(mcs::ABNF::URI::fragment(long_fragment).has_value());
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
