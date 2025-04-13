

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{
    using namespace mcs::abnf::uri; // NOLINT
    {
        // 空路径测试
        static_assert(mcs::abnf::uri::path_abempty(mcs::abnf::empty_span).has_value());
    }

    { // 仅斜杠测试
        static constexpr mcs::abnf::OCTET single_slash[] = {'/'};
        static_assert(mcs::abnf::uri::path_abempty(single_slash).has_value());
    }

    { // 多斜杠测试
        static constexpr mcs::abnf::OCTET multiple_slashes[] = {'/', '/', '/'};
        static_assert(mcs::abnf::uri::path_abempty(multiple_slashes).has_value());
    }

    { // 有效segment测试
        static constexpr mcs::abnf::OCTET valid_segments[] = {'/', 'a', '/',
                                                              'b', '/', 'c'};
        static_assert(mcs::abnf::uri::path_abempty(valid_segments).has_value());
    }

    { // 混合斜杠和segment测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {'/', 'a', 'b', '/', '/',
                                                           'c', 'd', '/', 'e'};
        static_assert(mcs::abnf::uri::path_abempty(mixed_valid).has_value());
    }

    { // 百分比编码segment测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'/', '%', '4', '1',
                                                           '/', '%', 'A', 'F'};
        static_assert(mcs::abnf::uri::path_abempty(pct_encoded).has_value());
    }

    { // 无效起始字符测试
        static constexpr mcs::abnf::OCTET invalid_start[] = {'a', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_abempty(invalid_start).has_value());
    }

    { // 包含无效字符的segment测试
        static constexpr mcs::abnf::OCTET invalid_segment[] = {'/', 'a', ' ', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_abempty(invalid_segment).has_value());
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'/', 'a', '%',
                                                              '2', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_abempty(incomplete_pct).has_value());
    }

    { // 长路径测试
        static constexpr mcs::abnf::OCTET long_path[] = {
            '/', 'a', 'b', 'c', '/', '1', '2', '3', '/', '!', '$', '&', '/',
            '%', '4', '1', '/', 'x', 'y', 'z', '/', '/', 'e', 'n', 'd'};
        static_assert(mcs::abnf::uri::path_abempty(long_path).has_value());
    }

    {                                                               // 边界情况测试
        static constexpr mcs::abnf::OCTET boundary1[] = {'/', 'a'}; // 单segment
        static constexpr mcs::abnf::OCTET boundary2[] = {'/', 'a', '/',
                                                         '/'}; // segment后多斜杠
        static_assert(mcs::abnf::uri::path_abempty(boundary1).has_value());
        static_assert(mcs::abnf::uri::path_abempty(boundary2).has_value());
    }
    return 0;
}
// NOLINTEND