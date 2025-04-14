

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"

// NOLINTBEGIN

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= relative-ref 有效用例 =================
    // 基础结构组合测试
    constexpr auto minimal_case = "doc.txt"_span; // 仅relative-part (path-noscheme)
    constexpr auto with_query = "//api/search?q=uri"_span; // relative-part + 查询
    {
        static_assert(relative_part("//api/search"_span));
        static_assert(query("q=uri"_span));
        static_assert(relative_part("//api/search"_span) && query("q=uri"_span));
    }
    constexpr auto with_fragment = "/data#section2"_span; // relative-part + 片段
    constexpr auto full_combo = "//user:pass@host:80/path?q=1#x"_span; // 全要素组合

    // 空查询/片段边界
    constexpr auto empty_query = "config.json?"_span;  // 空查询
    constexpr auto empty_fragment = "table#"_span;     // 空片段
    constexpr auto double_empty = "index?empty#"_span; // 空查询+空片段

    // 编码与特殊字符测试
    constexpr auto encoded_query =
        "search?filter=%E4%B8%AD%26%3D"_span;                 // 中文和保留字符编码
    constexpr auto special_fragment = "log#!$&'()*+,;="_span; // 片段使用sub-delims
    constexpr auto mixed_encoding =
        "file%20(1).md?name=a#b%20c"_span; // 混合路径/查询/片段编码

    // 路径类型全覆盖
    constexpr auto authority_path =
        "//example.com/img/logo.png"_span;                     // 类型1（authority+path）
    constexpr auto absolute_path = "/sys/../etc/passwd"_span;  // 类型2（path-absolute）
    constexpr auto noscheme_path = "folder;param=1/file"_span; // 类型3（path-noscheme）
    constexpr auto empty_path = ""_span;                       // 类型4（path-empty）

    static_assert(relative_ref(minimal_case));
    static_assert(relative_ref(with_query));
    static_assert(relative_ref(with_fragment));
    static_assert(relative_ref(full_combo));
    static_assert(relative_ref(empty_query));
    static_assert(relative_ref(empty_fragment));
    static_assert(relative_ref(double_empty));
    static_assert(relative_ref(encoded_query));
    static_assert(relative_ref(special_fragment));
    static_assert(relative_ref(mixed_encoding));
    static_assert(relative_ref(authority_path));
    static_assert(relative_ref(absolute_path));
    static_assert(relative_ref(noscheme_path));
    static_assert(relative_ref(empty_path));

    //================= relative-ref 无效用例 =================
    // 结构错误
    constexpr auto fragment_before_query = "data#x?q=1"_span; // 片段在查询前（违反顺序）
    constexpr auto multiple_query = "search?q=1&q=2?repeat"_span; // 多个问号
    constexpr auto multiple_fragment = "doc#v1#v2"_span;          // 多个井号

    // 非法字符
    constexpr auto unencoded_space = "file name.txt"_span; // 路径未编码空格
    constexpr auto invalid_query_char =
        "calc?expr=1+2=3"_span; // 查询中的等号需允许（保留字符）
    // 注：根据RFC3986查询允许保留字符，此用例可能为合法，需根据具体实现需求调整

    // 路径格式错误
    constexpr auto bad_authority = "//host:port/"_span; // 非数字端口（port非DIGIT）
    constexpr auto illegal_path = "/usr/bin|ls"_span;   // 路径含非法字符（|）
    constexpr auto noscheme_colon = "file:name"_span;   // path-noscheme含冒号（冲突）

    // 边界错误
    constexpr auto query_only = "?query"_span;       // 缺失relative-part
    constexpr auto fragment_only = "#top"_span;      // 缺失relative-part
    constexpr auto double_question = "??query"_span; // 多个问号开头

    static_assert(!relative_ref(fragment_before_query));
    static_assert(relative_ref(multiple_query)); // NOTE: relative-part empty is allow
    {
        static_assert(relative_part(empty_span_param));
    }
    static_assert(!relative_ref(multiple_fragment));
    static_assert(!relative_ref(unencoded_space));

    static_assert(relative_ref(invalid_query_char));
    {
        static_assert(relative_part("calc"_span));
        static_assert(query("expr=1+2=3"_span));
    }

    // 根据RFC此应为合法，根据实现需求启用
    static_assert(!relative_ref(bad_authority));
    static_assert(!relative_ref(illegal_path));
    static_assert(!relative_ref(noscheme_colon));

    static_assert(relative_ref(query_only));
    static_assert(relative_ref(fragment_only));
    static_assert(relative_ref(double_question));
    {
        static_assert(query("??"_span));
    }

    // 极端测试用例
    constexpr auto massive_encoding =
        "//[vF.foo]/%E2%82%AC?%23=%23#%25"_span; // 欧元符号编码+自引用测试
    constexpr auto nested_components =
        "/a/b/c/../.././d?x=1&y=2#/sub"_span; // 复杂路径标准化
    // constexpr auto max_length_case = ("x?q=" + std::string_view(5000, 'a')) +
    //                                  "#end"_span; // 超长查询（需处理长度限制）

    // 注意：max_length_case需要根据解析器能力调整，constexpr可能无法处理过长的字符串

    EXPECT(relative_ref(massive_encoding));
    static_assert(relative_ref(nested_components));

    return 0;
}

// NOLINTEND