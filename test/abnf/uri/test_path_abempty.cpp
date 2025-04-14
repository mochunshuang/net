

#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{
    using namespace mcs::abnf::uri; // NOLINT
    {
        // 空路径测试
        static_assert(mcs::abnf::uri::path_abempty(mcs::abnf::empty_span_param));
    }

    { // 仅斜杠测试
        static constexpr mcs::abnf::OCTET single_slash[] = {'/'};
        static_assert(mcs::abnf::uri::path_abempty(single_slash));
    }

    { // 多斜杠测试
        static constexpr mcs::abnf::OCTET multiple_slashes[] = {'/', '/', '/'};
        static_assert(mcs::abnf::uri::path_abempty(multiple_slashes));
    }

    { // 有效segment测试
        static constexpr mcs::abnf::OCTET valid_segments[] = {'/', 'a', '/',
                                                              'b', '/', 'c'};
        static_assert(mcs::abnf::uri::path_abempty(valid_segments));
    }

    { // 混合斜杠和segment测试
        static constexpr mcs::abnf::OCTET mixed_valid[] = {'/', 'a', 'b', '/', '/',
                                                           'c', 'd', '/', 'e'};
        static_assert(mcs::abnf::uri::path_abempty(mixed_valid));
    }

    { // 百分比编码segment测试
        static constexpr mcs::abnf::OCTET pct_encoded[] = {'/', '%', '4', '1',
                                                           '/', '%', 'A', 'F'};
        static_assert(mcs::abnf::uri::path_abempty(pct_encoded));
    }

    { // 无效起始字符测试
        static constexpr mcs::abnf::OCTET invalid_start[] = {'a', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_abempty(invalid_start));
    }

    { // 包含无效字符的segment测试
        static constexpr mcs::abnf::OCTET invalid_segment[] = {'/', 'a', ' ', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_abempty(invalid_segment));
    }

    { // 不完整的百分比编码测试
        static constexpr mcs::abnf::OCTET incomplete_pct[] = {'/', 'a', '%',
                                                              '2', '/', 'b'};
        static_assert(!mcs::abnf::uri::path_abempty(incomplete_pct));
    }

    { // 长路径测试
        static constexpr mcs::abnf::OCTET long_path[] = {
            '/', 'a', 'b', 'c', '/', '1', '2', '3', '/', '!', '$', '&', '/',
            '%', '4', '1', '/', 'x', 'y', 'z', '/', '/', 'e', 'n', 'd'};
        static_assert(mcs::abnf::uri::path_abempty(long_path));
    }

    {                                                               // 边界情况测试
        static constexpr mcs::abnf::OCTET boundary1[] = {'/', 'a'}; // 单segment
        static constexpr mcs::abnf::OCTET boundary2[] = {'/', 'a', '/',
                                                         '/'}; // segment后多斜杠
        static_assert(mcs::abnf::uri::path_abempty(boundary1));
        static_assert(mcs::abnf::uri::path_abempty(boundary2));
    }
    return 0;
}
// NOLINTEND