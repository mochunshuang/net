
#include "../test_head.hpp"

// NOLINTBEGIN

#include <cassert>
#include <iostream>

using namespace mcs::abnf;

int main()
{
    { // 有效unreserved字符测试
        static constexpr mcs::abnf::OCTET unreserved_chars[] = {'a', 'Z', '0', '-',
                                                                '.', '_', '~'};
        static_assert(mcs::abnf::uri::segment_nz_nc(unreserved_chars));
    }

    { // 有效sub-delims字符测试
        static constexpr mcs::abnf::OCTET subdelims_chars[] = {
            '!', '$', '&', '\'', '(', ')', '*', '+', ',', ';', '='};
        static_assert(mcs::abnf::uri::segment_nz_nc(subdelims_chars));
    }

    { // 有效@符号测试
        static constexpr mcs::abnf::OCTET at_sign[] = {'a', '@', 'b'};
        static_assert(mcs::abnf::uri::segment_nz_nc(at_sign));
    }

    { // 有效百分比编码测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'%', '4', '1', '@',
                                                           '%', 'A', 'F'};
        static_assert(mcs::abnf::uri::segment_nz_nc(pct_encoded));
    }

    { // 混合有效字符测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {'a', '!', '@', '1',
                                                           '$', '.', '(', '~'};
        static_assert(mcs::abnf::uri::segment_nz_nc(mixed_valid));
    }

    { // 空输入测试
        static_assert(!mcs::abnf::uri::segment_nz_nc(empty_span_param));
    }

    { // 无效字符测试
        static constexpr mcs::abnf::OCTET invalid_chars1[] = {' '};
        static constexpr mcs::abnf::OCTET invalid_chars2[] = {'#'};
        static constexpr mcs::abnf::OCTET invalid_chars3[] = {':'}; // 明确不允许冒号
        static_assert(!mcs::abnf::uri::segment_nz_nc(invalid_chars1));
        static_assert(!mcs::abnf::uri::segment_nz_nc(invalid_chars2));
        static_assert(!mcs::abnf::uri::segment_nz_nc(invalid_chars3));
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'a', '%', '2', '@', 'b'};
        static_assert(!mcs::abnf::uri::segment_nz_nc(incomplete_pct));
    }

    { // 无效百分比编码测试
        static constexpr mcs::abnf::OCTET invalid_pct[] = {'%', 'G', '0', '@', 'a'};
        static_assert(!mcs::abnf::uri::segment_nz_nc(invalid_pct));
    }

    {                                                          // 边界情况测试
        static constexpr mcs::abnf::OCTET boundary1[] = {'@'}; // 仅@符号
        static constexpr mcs::abnf::OCTET boundary2[] = {'a', '@', '!', '@',
                                                         '1'}; // 多个@符号
        static_assert(mcs::abnf::uri::segment_nz_nc(boundary1));
        static_assert(mcs::abnf::uri::segment_nz_nc(boundary2));
    }

    { // 长segment测试
        static constexpr mcs::abnf::OCTET long_segment[] = {'a', 'b', 'c', '@', '1', '2',
                                                            '3', '!', '$', '&', '%', '4',
                                                            '1', '@', 'x', 'y', 'z'};
        static_assert(mcs::abnf::uri::segment_nz_nc(long_segment));
    }
    std::cout << "main done\n";
    return 0;
}

// NOLINTEND
