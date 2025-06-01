#include "../test_head.hpp"

// NOLINTBEGIN

using namespace mcs::protocol::http::rules;

int main()
{
    // chunk-ext-val = token / quoted-string
    constexpr auto chunk_ext_val_pass = make_pass_test<chunk_ext_val>();
    constexpr auto chunk_ext_val_fail = make_unpass_test<chunk_ext_val>();

    static_assert(chunk_ext_val_pass("abc"_span));         // NOTE: 非双引号
    static_assert(chunk_ext_val_pass("\"\""_span));        // NOTE: 待引号，非双引号
    static_assert(chunk_ext_val_pass("\" adasd \""_span)); // NOTE: 双引号字符串

    // NOTE: \\r\\n 是 不可见的字符。 单个'\'才是 控制字符
    static_assert(chunk_ext_val_pass("\" avc\\r\\n \""_span)); // NOTE: 双引号带回车

    // NOTE: \r\n 能区别于 chunk_ext_val
    static_assert(!chunk_ext_val_fail("\" avc\r\n \""_span)); // 控制字符
    static_assert(!chunk_ext_val_fail("\r"_span));
    static_assert(!chunk_ext_val_fail("\n"_span));
    static_assert(!chunk_ext_val_fail("\r\n"_span));

    return 0;
}
// NOLINTEND