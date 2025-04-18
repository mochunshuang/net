
#include "../test_head.hpp"

// NOLINTBEGIN

#include <cassert>
#include <iostream>

using namespace mcs::abnf;

int main()
{
    { // 有效pchar字符测试
        static constexpr mcs::abnf::OCTET pchars_only[] = {'a', '1', '-', '_',
                                                           '.', '~', '!', '&'};
        static_assert(mcs::abnf::uri::segment_nz(pchars_only));
    }

    { // 有效百分比编码测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'%', '4', '1', '%', 'A', 'F'};
        static_assert(mcs::abnf::uri::segment_nz(pct_encoded));
    }

    { // 混合有效字符测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {'a', '!', '1', '$',
                                                           '.', '(', '~'};
        static_assert(mcs::abnf::uri::segment_nz(mixed_valid));
    }

    { // 空输入测试
        static_assert(!mcs::abnf::uri::segment_nz(empty_span_param));
    }

    { // 无效字符测试
        static constexpr mcs::abnf::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::abnf::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::abnf::OCTET invalid_chars3[] = {'['};
        static_assert(!mcs::abnf::uri::segment_nz(invalid_chars1));
        static_assert(!mcs::abnf::uri::segment_nz(invalid_chars2));
        static_assert(!mcs::abnf::uri::segment_nz(invalid_chars3));
    }

    { // 完整的百分比编码测试

        // query         = *( pchar / "/" / "?" )
        // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'a', '%', '2', 'b'};
        static_assert(mcs::abnf::uri::unreserved('a'));
        static_assert(mcs::abnf::uri::pct_encoded('%', '2', 'b'));
        static_assert(mcs::abnf::uri::segment_nz(incomplete_pct));
    }
    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'a', '%', '2'};
        static_assert(!mcs::abnf::uri::segment_nz(incomplete_pct));
    }

    { // 无效百分比编码测试
        static constexpr mcs::abnf::OCTET invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!mcs::abnf::uri::segment_nz(invalid_pct));
    }

    { // 单字符边界测试
        static constexpr mcs::abnf::OCTET single_char1[] = {'a'};
        static constexpr mcs::abnf::OCTET single_char2[] = {'!'};
        static constexpr mcs::abnf::OCTET single_char3[] = {'%', '4', '1'};
        static_assert(mcs::abnf::uri::segment_nz(single_char1));
        static_assert(mcs::abnf::uri::segment_nz(single_char2));
        static_assert(mcs::abnf::uri::segment_nz(single_char3));
    }

    { // 长segment测试
        static constexpr mcs::abnf::OCTET long_segment[] = {
            'a', 'b', 'c', '1', '2', '3', '!', '$', '&', '%', '4', '1', 'x', 'y', 'z'};
        static_assert(mcs::abnf::uri::segment_nz(long_segment));
    }

    { // 包含允许的特殊字符测试
        static constexpr mcs::abnf::OCTET special_chars[] = {':', '@'};
        static_assert(mcs::abnf::uri::segment_nz(special_chars));
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
