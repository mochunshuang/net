

#include "../test_head.hpp"

// NOLINTBEGIN

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= relative-part 有效用例 =================
    // 类型1：//authority + path-abempty
    constexpr auto type1_full = "//user@[v1.a]/api/v1"_span; // 含完整authority和路径
    {
        constexpr auto auth = "user@[v1.a]"_span;
        constexpr auto path = "/api/v1"_span;
        // authority     = [ userinfo "@" ] host [ ":" port ]
        EXPECT(authority(auth));
        EXPECT(path_abempty(path).has_value());
    }
    constexpr auto type1_min = "//example.com"_span;  // 最小authority（无路径）
    constexpr auto type1_empty_path = "//host:"_span; // 空port + 空路径

    // 类型2：path-absolute
    constexpr auto type2_basic = "/data/report.pdf"_span; // 标准绝对路径
    constexpr auto type2_deep = "/a/b/c/d/e/f/g"_span;    // 深层路径
    constexpr auto type2_special = "/!$&'()*+,;="_span;   // 包含允许的特殊字符

    // 类型3：path-noscheme
    constexpr auto type3_basic = "document.txt"_span;    // 简单无scheme路径
    constexpr auto type3_encoded = "file%20(1).md"_span; // 带编码字符
    constexpr auto type3_mixed = "a/b/c;param=1"_span;   // 含路径参数

    // 类型4：path-empty
    constexpr auto type4_empty = ""_span;    // 空路径（隐式空）
    constexpr auto type4_zero_len = ""_span; // 显式空字符串

    EXPECT(relative_part(type1_full));
    EXPECT(relative_part(type1_min));
    EXPECT(relative_part(type1_empty_path));
    static_assert(relative_part(type2_basic));
    static_assert(relative_part(type2_deep));
    static_assert(relative_part(type2_special));
    static_assert(relative_part(type3_basic));
    static_assert(relative_part(type3_encoded));
    static_assert(relative_part(type3_mixed));
    static_assert(relative_part(type4_empty));
    static_assert(relative_part(type4_zero_len));

    //================= relative-part 无效用例 =================
    // 结构错误：混合类型
    constexpr auto mixed_type1 = "//host/path/../noscheme"_span;
    {
        // path-abempty  = *( "/" segment )
        // segment       = *pchar
        // NOTE: segment 允许是 空的 因此 relative_part 是合法的
        static_assert(not pchar('/'));
        static_assert(not segment("/"_span));
        static_assert(not segment("//"_span));
        static_assert(path_abempty(mixed_type1));
        static_assert(path_abempty("/"_span));
        static_assert(path_abempty("//"_span));
    }
    constexpr auto mixed_type2 =
        "/absolute/path?query#frag"_span; // 含不允许的查询片段（需在URI层级处理）

    // 非法路径字符
    constexpr auto invalid_char1 = "//host/|pipe"_span; // 管道符非法
    constexpr auto invalid_char2 = "data[1].bin"_span;  // 方括号未编码

    // 格式错误
    constexpr auto single_slash = "/"_span; // 不是错误的
    {
        static_assert(path_absolute(single_slash));
    }
    constexpr auto triple_slash = "///ghost.txt"_span;        // 多个斜杠开头
    constexpr auto bad_authority = "//user@host:80port"_span; // port含字母

    // 路径类型误用
    constexpr auto noscheme_absolute = "//a/b/c"_span; // 类型1的path-abempty应允许斜杠
    constexpr auto absolute_with_colon = "/path:123"_span; // 路径段含非法冒号
    {
        static_assert(not userinfo(absolute_with_colon));
        static_assert(userinfo("path"_span));
        static_assert(userinfo("path"_span));

        // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
        // segment-nz    = 1*pchar
        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        static_assert(path_absolute(absolute_with_colon));
        static_assert(pchar(':')); // NOTE: 是允许 ":"在路径中的 rest风格好像
    }

    // 空值边界
    static_assert(relative_part(empty_span_param)); // 允许空

    static_assert(relative_part(mixed_type1));

    static_assert(!relative_part(mixed_type2));
    static_assert(!relative_part(invalid_char1));
    static_assert(!relative_part(invalid_char2));

    static_assert(relative_part(single_slash));

    static_assert(!relative_part(triple_slash));
    static_assert(!relative_part(bad_authority));

    static_assert(relative_part(noscheme_absolute)); // 应通过？需要确认path-abempty定义
    static_assert(relative_part(absolute_with_colon));

    // 特殊说明：noscheme_absolute需根据path-abempty定义判断合法性
    // 如果path-abempty允许"/b/c"形式，则此用例应为合法。此处假设为非法需具体实现确认

    return 0;
}

// NOLINTEND