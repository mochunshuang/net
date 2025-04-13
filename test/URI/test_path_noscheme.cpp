

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{
    using namespace mcs::abnf::uri; // NOLINT

    { // 单个segment-nz-nc测试
        static constexpr mcs::abnf::OCTET single_segment[] = {'a', 'b', '@', 'c'};
        static_assert(mcs::abnf::uri::path_noscheme(single_segment).has_value());
    }

    { // 多段路径测试
        static constexpr mcs::abnf::OCTET multi_segment[] = {'a', '@', 'b', '/',
                                                             'c', '/', 'd'};
        static_assert(mcs::abnf::uri::path_noscheme(multi_segment).has_value());
    }

    { // 包含百分比编码测试
        static constexpr mcs::abnf::OCTET with_encoding[] = {'a', '%', '4',
                                                             '1', '/', 'b'};
        static_assert(mcs::abnf::uri::path_noscheme(with_encoding).has_value());
    }

    { // 无效冒号字符测试
        static constexpr mcs::abnf::OCTET with_colon[] = {'a', ':', 'b'};
        static_assert(!mcs::abnf::uri::path_noscheme(with_colon).has_value());
    }

    { // 空输入测试
        static_assert(!mcs::abnf::uri::path_noscheme(mcs::abnf::empty_span).has_value());
    }

    { // 无效起始字符测试
        static constexpr mcs::abnf::OCTET invalid_start[] = {'/', 'a', 'b'};
        static_assert(!mcs::abnf::uri::path_noscheme(invalid_start).has_value());
    }

    { // 长路径测试
        static constexpr mcs::abnf::OCTET long_path[] = {
            'a', '!', '@', '1', '/', 'b', 'c', 'd', '/', '%',
            '4', '1', '/', '(', ')', '/', '2', '3', '4'};
        static_assert(mcs::abnf::uri::path_noscheme(long_path).has_value());
    }
    return 0;
}
// NOLINTEND