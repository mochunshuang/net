
#include "../test_head.hpp"

// NOLINTBEGIN

#include <array>
#include <iostream>

int main()
{
    { // 空输入测试
        static_assert(mcs::abnf::uri::pchars(mcs::abnf::empty_span_param));
    }

    { // 纯unreserved字符测试
        static constexpr mcs::abnf::OCTET unreserved_chars[] = {'a', 'Z', '0', '-',
                                                                '.', '_', '~'};
        static_assert(mcs::abnf::uri::pchars(unreserved_chars));
    }

    { // 纯sub-delims字符测试
        static constexpr mcs::abnf::OCTET subdelims_chars[] = {
            '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};
        static_assert(mcs::abnf::uri::pchars(subdelims_chars));
    }

    { // 混合字符测试
        static constexpr mcs::abnf::OCTET mixed_chars[] = {'a', '!', '1', '$',
                                                           '.', '(', '~'};
        static_assert(mcs::abnf::uri::pchars(mixed_chars));
    }

    { // 百分比编码测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'%', '4', '1', '%',
                                                           'A', 'F', 'a'};
        static_assert(mcs::abnf::uri::pchars(pct_encoded));
    }

    { // 无效字符测试
        static constexpr mcs::abnf::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::abnf::OCTET invalid_chars2[] = {'"'};
        static constexpr mcs::abnf::OCTET invalid_chars3[] = {'<'};
        static_assert(!mcs::abnf::uri::pchars(invalid_chars1));
        static_assert(!mcs::abnf::uri::pchars(invalid_chars2));
        static_assert(!mcs::abnf::uri::pchars(invalid_chars3));
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct1[] = {'%'};
        static constexpr mcs::abnf::OCTET incomplete_pct2[] = {'%', 'A'};
        static_assert(!mcs::abnf::uri::pchars(incomplete_pct1));
        static_assert(!mcs::abnf::uri::pchars(incomplete_pct2));
    }

    { // 无效百分比编码测试
        static constexpr mcs::abnf::OCTET invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!mcs::abnf::uri::pchars(invalid_pct));
    }

    { // 混合有效和无效字符测试
        static constexpr mcs::abnf::OCTET mixed_invalid[] = {'a', '!', ' ', '1'};
        static_assert(!mcs::abnf::uri::pchars(mixed_invalid));
    }

    {                                                               // 边界情况测试
        static constexpr mcs::abnf::OCTET boundary1[] = {':', '@'}; // 特殊允许字符
        static constexpr mcs::abnf::OCTET boundary2[] = {'#', '?'}; // gen-delims应该无效
        static_assert(mcs::abnf::uri::pchars(boundary1));
        static_assert(!mcs::abnf::uri::pchars(boundary2));
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
