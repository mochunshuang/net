

#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= absolute-URI 有效用例 =================
    // 基础结构组合测试
    constexpr auto minimal_valid = "scheme:/path"_span; // 最小结构（无查询）
    constexpr auto full_combo = "https://user@[::1]:8080/a/b?q=1&x=2"_span; // 全要素
    constexpr auto empty_hier = "g:?query-only"_span; // hier-part为空（path-empty）

    // 各hier-part类型覆盖
    constexpr auto type1_authority =
        "ftp://example.com"_span; // hier-part类型1（authority）
    constexpr auto type2_absolute =
        "file:/etc/passwd"_span; // hier-part类型2（path-absolute）
    constexpr auto type3_rootless =
        "test:segment:123"_span;            // hier-part类型3（path-rootless）
    constexpr auto type4_empty = "m:"_span; // hier-part类型4（path-empty）

    // 查询部分测试
    constexpr auto encoded_query = "api://host?%21%3D%26"_span; // 编码特殊字符（! = &）
    constexpr auto complex_query =
        "search?q=uri+parser&limit=100#ignored"_span; // 含非法#但被上层过滤
    {
        // NOTE: 必须要编码
        // absolute-URI  = scheme ":" hier-part [ "?" query ]
        static_assert(not query("q=uri+parser&limit=100#ignored"_span));
    }
    constexpr auto empty_query = "data:?"_span; // 空查询
    // constexpr auto long_query = "s:" + string_view(500,'a') + "?q=" +
    // string_view(1000,'b'_span;

    // 特殊scheme格式
    constexpr auto uppercase_scheme = "MAILTO:user@domain"_span; // 大写scheme
    constexpr auto dotted_scheme = "com.example.data:/a"_span;   // 带点的scheme名

    static_assert(absolute_URI(minimal_valid));
    static_assert(absolute_URI(full_combo));
    static_assert(absolute_URI(empty_hier));
    static_assert(absolute_URI(type1_authority));
    static_assert(absolute_URI(type2_absolute));
    static_assert(absolute_URI(type3_rootless));
    static_assert(absolute_URI(type4_empty));
    static_assert(absolute_URI(encoded_query));
    static_assert(not absolute_URI(complex_query)); // 需确保#被上层解析器过滤
    static_assert(absolute_URI(empty_query));
    // static_assert(absolute_URI(long_query));   // 运行时测试（constexpr可能不支持）

    //================= absolute-URI 无效用例 =================
    // 结构错误
    constexpr auto missing_colon = "http//host"_span;  // 缺少冒号
    constexpr auto double_colon = "scheme::path"_span; // 多个冒号
    constexpr auto fragment_attempt = "s:/a#x"_span;   // 含非法片段

    // 非法scheme
    constexpr auto num_scheme = "9news:data"_span;           // 数字开头的scheme
    constexpr auto invalid_scheme_char = "scheme%20:/"_span; // scheme含非法字符

    // hier-part错误
    constexpr auto bad_authority = "s://host:port"_span;      // 非数字端口
    constexpr auto invalid_rootless = "t:seg ment"_span;      // 未编码空格
    constexpr auto absolute_path_has_colon = "x:/pa:th"_span; // path-absolute段含冒号
    {
        static_assert(scheme("x"_span));
        // path-absolute = "/" [ segment-nz *( "/" segment ) ]
        // segment-nz    = 1*pchar
        static_assert(path_absolute("/pa:th"_span)); // NOTE: ':' 是允许的
        static_assert(pchar(':'));
    }
    // 查询部分错误
    constexpr auto unencoded_space_query = "q:/?a b"_span; // 查询未编码空格
    constexpr auto control_char_query = "x:?a\x01b"_span;  // 控制字符

    static_assert(!absolute_URI(missing_colon));

    static_assert(absolute_URI(double_colon));

    static_assert(!absolute_URI(fragment_attempt));
    static_assert(!absolute_URI(num_scheme));
    static_assert(!absolute_URI(invalid_scheme_char));
    static_assert(!absolute_URI(bad_authority));
    static_assert(!absolute_URI(invalid_rootless));

    static_assert(absolute_URI(absolute_path_has_colon));

    static_assert(!absolute_URI(unencoded_space_query));
    static_assert(!absolute_URI(control_char_query));

    // 极端测试用例
    // constexpr auto scheme_max_length =
    //     "a123+.-zzz:" + string_view(5000, '/') + "?x"_span; // 超长scheme
    // constexpr auto hier_part_overflow =
    //     "s:" + string_view(10000, 'a') + "?q="_span;              // 超长hier-part
    constexpr auto nested_encoding = "s:%2F%2Fhost?%23frag"_span; // 编码关键字符（//和#）

    static_assert(absolute_URI(nested_encoding)); // 应合法（编码后无特殊含义）
    // static_assert(!absolute_URI(scheme_max_length)); // 长度限制依赖实现

    return 0;
}

// NOLINTEND