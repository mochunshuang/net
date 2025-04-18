
#include "../test_head.hpp"

// NOLINTBEGIN

#include <cassert>
#include <iostream>

using namespace mcs::abnf;

int main()
{
    { // 空片段测试
        static_assert(mcs::abnf::uri::fragment(empty_span_param));
    }

    { // 纯pchar字符测试
        static constexpr mcs::abnf::OCTET pchars_only[] = {'a', '1', '-', '_',
                                                           '.', '~', '!', '&'};
        static_assert(mcs::abnf::uri::fragment(pchars_only));
    }

    { // 包含斜杠测试
        static constexpr mcs::abnf::OCTET with_slash[] = {'a', '/', 'b', '/', 'c'};
        static_assert(mcs::abnf::uri::fragment(with_slash));
    }

    { // 包含问号测试
        static constexpr mcs::abnf::OCTET with_question[] = {'q', '?', 'p', 'a', 'r',
                                                             'a', 'm', '=', '1'};
        static_assert(mcs::abnf::uri::fragment(with_question));
    }

    { // 百分比编码测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'%', '2', '0', '/',
                                                           '%', '3', 'F', '?'};
        static_assert(mcs::abnf::uri::fragment(pct_encoded));
    }

    { // 混合有效字符测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {
            's', 'e', 'c', 't', 'i', 'o', 'n', '/', '1', '?', 'a', '=', 'b'};
        static_assert(mcs::abnf::uri::fragment(mixed_valid));
    }

    { // 无效字符测试
        static constexpr mcs::abnf::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::abnf::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::abnf::OCTET invalid_chars3[] = {'['};
        static_assert(!mcs::abnf::uri::fragment(invalid_chars1));
        static_assert(!mcs::abnf::uri::fragment(invalid_chars2));
        static_assert(!mcs::abnf::uri::fragment(invalid_chars3));
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct1[] = {'a', '%', '2', 'b'};
        static constexpr mcs::abnf::OCTET incomplete_pct2[] = {'%', 'A'};
        static_assert(mcs::abnf::uri::fragment(incomplete_pct1));
        static_assert(!mcs::abnf::uri::fragment(incomplete_pct2));
    }

    { // 无效百分比编码测试
        static constexpr mcs::abnf::OCTET invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!mcs::abnf::uri::fragment(invalid_pct));
    }

    {                                                                    // 边界情况测试
        static constexpr mcs::abnf::OCTET boundary1[] = {'/', '?', 'a'}; // 特殊字符组合
        static constexpr mcs::abnf::OCTET boundary2[] = {'?', '/', '%', '4',
                                                         '1'}; // 混合特殊字符和编码
        static_assert(mcs::abnf::uri::fragment(boundary1));
        static_assert(mcs::abnf::uri::fragment(boundary2));
    }

    { // 长片段测试
        static constexpr mcs::abnf::OCTET long_fragment[] = {
            's', 'e', 'c', 't', 'i', 'o', 'n', '/', '1', '/', '2', '?',
            'p', 'a', 'r', 'a', 'm', '=', 'v', 'a', 'l', '%', '2', '0'};
        static_assert(mcs::abnf::uri::fragment(long_fragment));
    }
    {
        { // 合法片段：包含编码后的# (%23)
            static constexpr OCTET encoded_hash[] = {'f', 'r', '%', '2', '3', 'a', 'g'};
            static_assert(mcs::abnf::uri::fragment(encoded_hash));
        }

        { // 非法片段：直接包含#
            static constexpr OCTET raw_hash[] = {'f', 'r', '#', 'a', 'g'};
            static_assert(!mcs::abnf::uri::fragment(raw_hash));
        }
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
