
#include "../test_head.hpp"

// NOLINTBEGIN

#include <array>
#include <iostream>

int main()
{
    { // 空segment测试
        static_assert(mcs::abnf::uri::segment(mcs::abnf::empty_span_param).has_value());
    }

    { // 纯pchar字符测试
        static constexpr mcs::abnf::OCTET pchars_only[] = {'a', '1', '-', '_',
                                                           '.', '~', '!', '&'};
        static_assert(mcs::abnf::uri::segment(pchars_only).has_value());
    }

    { // 百分比编码测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'%', '4', '1', '%', 'A', 'F'};
        static_assert(mcs::abnf::uri::segment(pct_encoded).has_value());
    }

    { // 混合有效字符测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {'a', '!', '1', '$',
                                                           '.', '(', '~'};
        static_assert(mcs::abnf::uri::segment(mixed_valid).has_value());
    }

    { // 包含允许的特殊字符测试
        static constexpr mcs::abnf::OCTET special_chars[] = {':', '@'};
        static_assert(mcs::abnf::uri::segment(special_chars).has_value());
    }

    { // 无效字符测试
        static constexpr mcs::abnf::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::abnf::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::abnf::OCTET invalid_chars3[] = {'['};
        static_assert(!mcs::abnf::uri::segment(invalid_chars1).has_value());
        static_assert(!mcs::abnf::uri::segment(invalid_chars2).has_value());
        static_assert(!mcs::abnf::uri::segment(invalid_chars3).has_value());
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'a', '%', 'g', '9'};
        static_assert(!mcs::abnf::uri::segment(incomplete_pct).has_value());
    }

    { // 无效百分比编码测试
        static constexpr mcs::abnf::OCTET invalid_pct[] = {'%', 'G', '0', 'a'};
        static_assert(!mcs::abnf::uri::segment(invalid_pct).has_value());
    }

    { // 单字符边界测试
        static constexpr mcs::abnf::OCTET single_char1[] = {'a'};
        static constexpr mcs::abnf::OCTET single_char2[] = {'!'};
        static constexpr mcs::abnf::OCTET single_char3[] = {'%', '4', '1'};
        static_assert(mcs::abnf::uri::segment(single_char1).has_value());
        static_assert(mcs::abnf::uri::segment(single_char2).has_value());
        static_assert(mcs::abnf::uri::segment(single_char3).has_value());
    }

    { // 长segment测试
        static constexpr mcs::abnf::OCTET long_segment[] = {'a', 'b', 'c', '1', '2', '3',
                                                            '!', '$', '&', '%', '4', '1',
                                                            'x', 'y', 'z', ':', '@'};
        static_assert(mcs::abnf::uri::segment(long_segment).has_value());
    }

    { // 混合有效和无效字符测试
        static constexpr mcs::abnf::OCTET mixed_invalid[] = {'a', '!', ' ', '1'};
        static_assert(!mcs::abnf::uri::segment(mixed_invalid).has_value());
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
