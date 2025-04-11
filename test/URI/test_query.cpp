
#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

#include <array>
#include <cassert>
#include <iostream>

using namespace mcs::ABNF;

int main()
{
    { // 空查询测试
        static_assert(mcs::ABNF::URI::query(mcs::ABNF::empty_span).has_value());
    }

    { // 纯pchar字符测试
        static constexpr mcs::ABNF::OCTET pchars_only[] = {'a', '1', '-', '_',
                                                           '.', '~', '!', '&'};
        static_assert(mcs::ABNF::URI::query(pchars_only).has_value());
    }

    { // 包含斜杠测试
        static constexpr std::array<mcs::ABNF::OCTET, 5> with_slash = {'a', '/', 'b', '/',
                                                                       'c'};
        static_assert(mcs::ABNF::URI::query(with_slash).has_value());
    }

    { // 包含问号测试
        static constexpr mcs::ABNF::OCTET with_question[] = {'q', '?', 'p', 'a', 'r',
                                                             'a', 'm', '=', '1'};
        static_assert(mcs::ABNF::URI::query(with_question).has_value());
    }

    { // 混合pchar、斜杠和问号测试
        static constexpr mcs::ABNF::OCTET mixed_valid[] = {
            's', 'e', 'a', 'r', 'c', 'h', '?', 'q', '=', '1', '&', 'p', '/', '2'};
        static_assert(mcs::ABNF::URI::query(mixed_valid).has_value());
    }

    { // 百分比编码测试
        static constexpr mcs::ABNF::OCTET pct_encoded[] = {'%', '2', '0', '/',
                                                           '%', '3', 'F', '?'};
        static_assert(mcs::ABNF::URI::query(pct_encoded).has_value());
    }

    { // 无效字符测试
        static constexpr mcs::ABNF::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::ABNF::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::ABNF::OCTET invalid_chars3[] = {'['};
        static_assert(!mcs::ABNF::URI::query(invalid_chars1).has_value());
        static_assert(!mcs::ABNF::URI::query(invalid_chars2).has_value());
        static_assert(!mcs::ABNF::URI::query(invalid_chars3).has_value());
    }

    { // 完整的百分比编码测试

        // query         = *( pchar / "/" / "?" )
        // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
        static constexpr mcs::ABNF::OCTET incomplete_pct[] = {'a', '%', '2', 'b'};
        static_assert(mcs::ABNF::URI::unreserved('a'));
        static_assert(mcs::ABNF::URI::pct_encoded('%', '2', 'b'));
        static_assert(mcs::ABNF::URI::query(incomplete_pct).has_value());
    }
    { // 不完整的百分比编码测试
        static constexpr mcs::ABNF::OCTET incomplete_pct[] = {'a', '%', '2'};
        static_assert(not mcs::ABNF::URI::query(incomplete_pct).has_value());
    }

    { // 混合有效和无效字符测试
        static constexpr mcs::ABNF::OCTET mixed_invalid[] = {'a', '/', ' ', '?', 'b'};
        static_assert(!mcs::ABNF::URI::query(mixed_invalid).has_value());
    }

    {
        // 边界情况测试
        static constexpr mcs::ABNF::OCTET boundary1[] = {'/', '?'};      // 仅特殊字符
        static constexpr mcs::ABNF::OCTET boundary2[] = {'?', '/', 'a'}; // 特殊字符组合
        static_assert(mcs::ABNF::URI::query(boundary1).has_value());
        static_assert(mcs::ABNF::URI::query(boundary2).has_value());
    }

    { // 长查询字符串测试
        static constexpr mcs::ABNF::OCTET long_query[] = {
            'a', '=', '1', '&', 'b', '=', '2', '&', 'c', '=', '3', '/', 'p',
            'a', 't', 'h', '?', 'q', 'u', 'e', 'r', 'y', '=', 'v', 'a', 'l'};
        static_assert(mcs::ABNF::URI::query(long_query).has_value());
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
