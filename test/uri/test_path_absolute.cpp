

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{
    using namespace mcs::abnf::uri; // NOLINT

    {
        // 空路径测试
        static_assert(
            !mcs::abnf::uri::path_absolute(mcs::abnf::empty_span_param).has_value());
    }

    { // 仅斜杠测试
        static constexpr mcs::abnf::OCTET slash_only[] = {'/'};
        static_assert(mcs::abnf::uri::path_absolute(slash_only).has_value());
    }

    { // 斜杠+segment-nz测试
        static constexpr mcs::abnf::OCTET with_segment[] = {'/', 'a', 'b', 'c'};
        static_assert(mcs::abnf::uri::path_absolute(with_segment).has_value());
    }

    { // 多段路径测试
        static constexpr mcs::abnf::OCTET multi_segment[] = {'/', 'a', '/',
                                                             'b', '/', 'c'};
        static_assert(mcs::abnf::uri::path_absolute(multi_segment).has_value());
    }

    { // 包含百分比编码测试
        static constexpr mcs::abnf::OCTET with_encoding[] = {'/', '%', '4',
                                                             '1', '/', 'b'};
        static_assert(mcs::abnf::uri::path_absolute(with_encoding).has_value());
    }

    { // 无效起始字符测试
        static constexpr mcs::abnf::OCTET invalid_start[] = {'a', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_absolute(invalid_start).has_value());
    }

    { // 空segment-nz测试
        static constexpr mcs::abnf::OCTET empty_segment[] = {'/', '/', 'a'};
        static_assert(!mcs::abnf::uri::path_absolute(empty_segment).has_value());
    }

    { // 长路径测试
        static constexpr mcs::abnf::OCTET long_path[] = {'/', 'a', 'b', '/', 'c', 'd',
                                                         'e', '/', '%', '4', '1', '/',
                                                         '!', '@', '/', '1', '2', '3'};
        static_assert(mcs::abnf::uri::path_absolute(long_path).has_value());
    }
    return 0;
}
// NOLINTEND