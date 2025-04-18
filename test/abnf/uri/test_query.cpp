
#include "../test_head.hpp"

// NOLINTBEGIN

#include <array>
#include <cassert>
#include <iostream>

using namespace mcs::abnf;

int main()
{
    { // 空查询测试
        static_assert(mcs::abnf::uri::query(mcs::abnf::empty_span_param));
    }

    { // 纯pchar字符测试
        static constexpr mcs::abnf::OCTET pchars_only[] = {'a', '1', '-', '_',
                                                           '.', '~', '!', '&'};
        static_assert(mcs::abnf::uri::query(pchars_only));
    }

    { // 包含斜杠测试
        static constexpr std::array<mcs::abnf::OCTET, 5> with_slash = {'a', '/', 'b', '/',
                                                                       'c'};
        static_assert(mcs::abnf::uri::query(with_slash));
    }

    { // 包含问号测试
        static constexpr mcs::abnf::OCTET with_question[] = {'q', '?', 'p', 'a', 'r',
                                                             'a', 'm', '=', '1'};
        static_assert(mcs::abnf::uri::query(with_question));
    }

    { // 混合pchar、斜杠和问号测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {
            's', 'e', 'a', 'r', 'c', 'h', '?', 'q', '=', '1', '&', 'p', '/', '2'};
        static_assert(mcs::abnf::uri::query(mixed_valid));
    }

    { // 百分比编码测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'%', '2', '0', '/',
                                                           '%', '3', 'F', '?'};
        static_assert(mcs::abnf::uri::query(pct_encoded));
    }

    { // 无效字符测试
        static constexpr mcs::abnf::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::abnf::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::abnf::OCTET invalid_chars3[] = {'['};
        static_assert(!mcs::abnf::uri::query(invalid_chars1));
        static_assert(!mcs::abnf::uri::query(invalid_chars2));
        static_assert(!mcs::abnf::uri::query(invalid_chars3));
    }

    { // 完整的百分比编码测试

        // query         = *( pchar / "/" / "?" )
        // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'a', '%', '2', 'b'};
        static_assert(mcs::abnf::uri::unreserved('a'));
        static_assert(mcs::abnf::uri::pct_encoded('%', '2', 'b'));
        static_assert(mcs::abnf::uri::query(incomplete_pct));
    }
    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'a', '%', '2'};
        static_assert(not mcs::abnf::uri::query(incomplete_pct));
    }

    { // 混合有效和无效字符测试
        static constexpr mcs::abnf::OCTET mixed_invalid[] = {'a', '/', ' ', '?', 'b'};
        static_assert(!mcs::abnf::uri::query(mixed_invalid));
    }

    {
        // 边界情况测试
        static constexpr mcs::abnf::OCTET boundary1[] = {'/', '?'};      // 仅特殊字符
        static constexpr mcs::abnf::OCTET boundary2[] = {'?', '/', 'a'}; // 特殊字符组合
        static_assert(mcs::abnf::uri::query(boundary1));
        static_assert(mcs::abnf::uri::query(boundary2));
    }

    { // 长查询字符串测试
        static constexpr mcs::abnf::OCTET long_query[] = {
            'a', '=', '1', '&', 'b', '=', '2', '&', 'c', '=', '3', '/', 'p',
            'a', 't', 'h', '?', 'q', 'u', 'e', 'r', 'y', '=', 'v', 'a', 'l'};
        static_assert(mcs::abnf::uri::query(long_query));
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
