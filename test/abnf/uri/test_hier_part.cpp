

#include "../test_head.hpp"

#include <cassert>

// NOLINTBEGIN

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= hier-part 有效用例 =================
    // 类型1：//authority + path-abempty
    constexpr auto type1_full =
        "//user:pass@[v1.a]:80/path/sub"_span;        // 完整authority和路径
    constexpr auto type1_min = "//example.com"_span;  // 最小authority（无路径）
    constexpr auto type1_empty_path = "//host:"_span; // 空port + 空路径

    // 类型2：path-absolute
    constexpr auto type2_basic = "/data/report.pdf"_span; // 标准绝对路径
    constexpr auto type2_traversal =
        "/../safe/../path"_span;                          // 包含..但符合语法（标准化前）
    constexpr auto type2_special = "/!$&'()*+,;=/_"_span; // 包含允许的特殊字符

    // 类型3：path-rootless
    constexpr auto type3_basic = "segment:123"_span;       // 无斜杠的根less路径
    constexpr auto type3_multi = "a/b/c;param"_span;       // 多段路径
    constexpr auto type3_encoded = "file%20name%3A1"_span; // 编码字符（空格和冒号）

    // 类型4：path-empty
    constexpr auto type4_empty = ""_span;            // 空字符串
    constexpr auto type4_with_scheme = "http:"_span; // 与scheme组合时的空路径

    EXPECT(hier_part(type1_full));
    static_assert(hier_part(type1_min));
    static_assert(hier_part(type1_empty_path));
    static_assert(hier_part(type2_basic));
    static_assert(hier_part(type2_traversal));
    static_assert(hier_part(type2_special));
    static_assert(hier_part(type3_basic));
    static_assert(hier_part(type3_multi));
    static_assert(hier_part(type3_encoded));
    static_assert(hier_part(type4_empty));
    static_assert(hier_part(type4_with_scheme));

    //================= hier-part 无效用例 =================
    // 结构冲突
    constexpr auto mixed_types =
        "//host/path/rootless"_span; // 类型1后接rootless路径（非法）
    {
        // hier-part     = "//" authority path-abempty
        static_assert(host("host"_span));
        // path-abempty  = *( "/" segment )
        static_assert(path_abempty("/path/rootless"_span)); // NOTE:是合法的
    }
    constexpr auto rootless_start =
        "/rootless"_span; // 以/开头的rootless路径（应为path-absolute）
    {
        // NOTE: path_abempty 几乎没有阻碍
        static_assert(path_abempty(rootless_start));
    }
    constexpr auto double_slash_rootless = "//rootless"_span; // 类型1缺少authority

    // 非法字符
    constexpr auto illegal_in_authority = "//host^name/"_span; // authority含非法字符^
    constexpr auto unencoded_space = "/path with space"_span;  // 路径未编码空格
    constexpr auto invalid_rootless = "seg{ment}"_span;        // 含大括号非法字符

    // 格式错误
    constexpr auto empty_authority = "///path"_span;  // 空authority（三斜杠）
    constexpr auto bad_port = "//host:portnum"_span;  // 非数字端口
    constexpr auto colon_in_absolute = "/pa:th"_span; // path-absolute段含冒号

    // 类型混淆
    constexpr auto empty_as_rootless = ""_span;      // 空字符串应属于path-empty
    constexpr auto rootless_with_query = "a?b"_span; // 查询组件属于上层URI结构

    static_assert(hier_part(mixed_types));

    static_assert(hier_part(rootless_start));

    static_assert(hier_part(double_slash_rootless));

    static_assert(!hier_part(illegal_in_authority));
    static_assert(!hier_part(unencoded_space));
    static_assert(!hier_part(invalid_rootless));
    static_assert(!hier_part(empty_authority));
    static_assert(!hier_part(bad_port));

    static_assert(hier_part(colon_in_absolute));
    {
        static_assert(path_abempty(colon_in_absolute));
    }

    static_assert(!hier_part(rootless_with_query));

    // 极端测试用例
    // constexpr auto massive_path = "//a/" + string_view(1000, 'a') +
    //                               "_span; // 超长路径（需处理缓冲区）

    constexpr auto encoded_authority =
        "//%75ser@host.%65x%61mple"_span; // 全编码authority

    constexpr auto rootless_edge = "::::::::::"_span; // 全冒号（语法合法但语义可能无效）

    static_assert(hier_part(encoded_authority)); // 验证编码解析
    static_assert(hier_part(rootless_edge));     // 纯冒号符合path-rootless语法
    // static_assert(!hier_part(massive_path));     // 长度限制需实现定义

    // 类型优先级测试
    constexpr auto empty_vs_absolute = "/"_span; // 应识别为path-absolute而非empty
    static_assert(hier_part(empty_vs_absolute));

    return 0;
}

// NOLINTEND