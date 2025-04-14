

#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{
    using namespace mcs::abnf::uri; // NOLINT

    { // 单个segment-nz-nc测试
        static constexpr mcs::abnf::OCTET single_segment[] = {'a', 'b', '@', 'c'};
        static_assert(mcs::abnf::uri::path_noscheme(single_segment));
    }

    { // 多段路径测试
        static constexpr mcs::abnf::OCTET multi_segment[] = {'a', '@', 'b', '/',
                                                             'c', '/', 'd'};
        static_assert(mcs::abnf::uri::path_noscheme(multi_segment));
    }

    { // 包含百分比编码测试
        static constexpr mcs::abnf::OCTET with_encoding[] = {'a', '%', '4',
                                                             '1', '/', 'b'};
        static_assert(mcs::abnf::uri::path_noscheme(with_encoding));
    }

    { // 无效冒号字符测试
        static constexpr mcs::abnf::OCTET with_colon[] = {'a', ':', 'b'};
        static_assert(!mcs::abnf::uri::path_noscheme(with_colon));
    }

    { // 空输入测试
        static_assert(!mcs::abnf::uri::path_noscheme(mcs::abnf::empty_span_param));
    }

    { // 无效起始字符测试
        static constexpr mcs::abnf::OCTET invalid_start[] = {'/', 'a', 'b'};
        static_assert(!mcs::abnf::uri::path_noscheme(invalid_start));
    }

    { // 长路径测试
        static constexpr mcs::abnf::OCTET long_path[] = {
            'a', '!', '@', '1', '/', 'b', 'c', 'd', '/', '%',
            '4', '1', '/', '(', ')', '/', '2', '3', '4'};
        static_assert(mcs::abnf::uri::path_noscheme(long_path));
    }
    return 0;
}
// NOLINTEND