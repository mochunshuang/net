

#include "../test_head.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <string_view>

// NOLINTBEGIN

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::abnf;
    using namespace mcs::abnf::uri; // NOLINT

    //================= uri 有效用例 =================
    // 全要素组合测试
    constexpr auto full_uri =
        "https://user:pass@[v1.a]:80/path?q=1#frag"_span;             // 所有组件
    constexpr auto encoded_uri = "scheme%2E:/a%20b?%23=%26#%25"_span; // 全组件编码测试
    {
        // scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
        static_assert(not scheme("scheme%2E"_span));
        static_assert(not ALPHA('%'));
        static_assert(not DIGIT('%'));
    }

    // 各hier-part类型覆盖
    constexpr auto authority_uri = "ftp://example.com/file.txt"_span; // 类型1 hier-part
    constexpr auto absolute_uri = "file:/usr/bin/sh"_span;            // 类型2 hier-part
    constexpr auto rootless_uri = "test:segment:123?x=1#y"_span;      // 类型3 hier-part
    constexpr auto empty_hier_uri = "m:#fragment"_span;               // 类型4 hier-part

    // 边界/极端值测试
    constexpr auto max_port_uri = "http://host:65535/path"_span; // 最大合法端口
    constexpr auto minimal_uri = "a:b"_span;                     // 最小合法uri
    constexpr auto empty_query_frag = "s:/?#"_span;              // 空查询+空片段
    // constexpr auto massive_query =
    //     "s:/?" + string_view(5000, 'a') + "#end"_span; // 超长查询

    // 特殊字符处理
    constexpr auto delims_in_components =
        "s://!$&'()*+,;=@host/path?/?#/?"_span; // 各组件允许的特殊字符

    {
        auto v = URI(full_uri);
        HAS_VALUE(v);
        {
            // "https://user:pass@[v1.a]:80/path?q=1#frag"_span;
            // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
            auto [s, h, q, f] = v.value();
            auto spn0 = full_uri.subspan(s.start, s.count);
            EXPECT(std::string(spn0.begin(), spn0.end()) == "https");
            {
                // NOTE: 工具测试
                REQUIRE_TRUE(equal_value(make_stdspan(full_uri, s), "https"),
                             "must equal https");
            }

            auto spn1 = full_uri.subspan(h.start, h.count);
            EXPECT(std::string(spn1.begin(), spn1.end()) == "//user:pass@[v1.a]:80/path");

            auto spn2 = full_uri.subspan(q.start, q.count);
            EXPECT(std::string(spn2.begin(), spn2.end()) == "q=1");

            auto spn3 = full_uri.subspan(f.start, f.count);
            EXPECT(std::string(spn3.begin(), spn3.end()) == "frag");

            auto str = std::string_view("https");
            assert(str.size() == spn0.size());
            for (std::size_t i = 0; i < spn0.size(); ++i)
            {
                assert(static_cast<OCTET>(str[i]) == spn0[i]);
            }
            {
                // NOTE:  string_view 天生是字符序列 和 span 可以匹配
                constexpr const char *c = "%3D%26";
                constexpr auto str = std::string_view(c);
                constexpr std::size_t size = str.size();
                std::cout << "str: " << str << '\n'; // str: %3D%26
                constexpr auto arr =
                    std::array<OCTET, size>{'%', '3', 'D', '%', '2', '6'};
                const std::span<const OCTET> span = arr; // NOTE: 不要引用或指向 右值
                for (std::size_t i = 0; i < size; ++i)
                {
                    EXPECT(static_cast<OCTET>(str[i]) == span[i]);
                }

                // 统一处理
                REQUIRE_TRUE(equal_value(span, str), "must equal_value span and str");
                REQUIRE_TRUE(equal_span(span, span), "must equal two span");
            }
        }
    }
    static_assert(not URI(encoded_uri));
    static_assert(URI(authority_uri));
    static_assert(URI(absolute_uri));
    static_assert(URI(rootless_uri));
    static_assert(URI(empty_hier_uri));
    {
        // constexpr auto empty_hier_uri = "m:#fragment"_span;
        auto v = URI(empty_hier_uri);
        auto [s, h, q, f] = v.value();
        REQUIRE_TRUE(equal_value(make_stdspan(empty_hier_uri, s), "m"),
                     "must equal_value m");

        REQUIRE_TRUE(h != invalid_span, "must equal_value");
        REQUIRE_TRUE(tool::is_valid_span(h));
        // NOTE: empty_span is valid_span
        REQUIRE_TRUE(tool::is_empty_span(h));

        // NOTE: is_valid_span mean not set value
        REQUIRE_FALSE(tool::is_valid_span(q));

        // for span{}
        REQUIRE_FALSE(tool::is_valid_span(span{}));
        REQUIRE_TRUE(tool::is_empty_span(span{}));

        REQUIRE_TRUE(tool::is_valid_span(f));
        {
            auto fra = tool::make_stdspan(empty_hier_uri, f);
            auto str = std::string_view("fragment");
            REQUIRE_TRUE(fra.size() == str.size(), "fra.size() == str.size()");
        }
        REQUIRE_TRUE(tool::equal_value(tool::make_stdspan(empty_hier_uri, f), "fragment"),
                     "f is fragment");
    }
    static_assert(URI(max_port_uri));
    static_assert(URI(minimal_uri));
    static_assert(URI(empty_query_frag));
    // static_assert(URI(massive_query)); // 运行时测试
    static_assert(URI(delims_in_components));

    //================= uri 无效用例 =================
    // 结构错误
    constexpr auto no_scheme = "//host/path"_span;                 // 缺少scheme和冒号
    constexpr auto double_colon = "scheme::path"_span;             // 多个冒号
    constexpr auto fragment_before_query = "s:/p#frag?query"_span; // 片段在查询前

    // 非法字符渗透
    constexpr auto space_in_scheme = "sch eme:/path"_span;  // scheme含空格
    constexpr auto unencoded_space = "s:/a path#frag"_span; // hier-part未编码空格
    constexpr auto control_char = "s:/pa\x01th?q=1"_span;   // 控制字符

    // 组件格式错误
    constexpr auto invalid_port = "http://host:80port/path"_span; // 端口含字母
    constexpr auto bad_path_rootless = "s:seg ment"_span;    // rootless路径未编码空格
    constexpr auto illegal_fragment_char = "s:/#frag%"_span; // 片段含未编码%

    // 协议规范冲突
    constexpr auto uri_without_hier =
        "mailto:user@domain.com"_span;                   // mailto属于uri但不是URL
    constexpr auto urn_like = "urn:issn:1535-3613"_span; // URN格式（需特殊处理）

    static_assert(!URI(no_scheme));
    static_assert(URI(double_colon));

    static_assert(!URI(fragment_before_query));
    static_assert(!URI(space_in_scheme));
    static_assert(!URI(unencoded_space));
    static_assert(!URI(control_char));
    static_assert(!URI(invalid_port));
    static_assert(!URI(bad_path_rootless));
    static_assert(!URI(illegal_fragment_char));

    static_assert(URI(uri_without_hier)); // 根据RFC3986应为合法，需确认需求
    static_assert(URI(urn_like));         // 根据RFC应为合法，需确认解析范围

    // 特殊说明测试
    constexpr auto tricky_encoding =
        "s://%2575ser@host?%3D%26#%25%23"_span;                       // 双重编码测试
    constexpr auto ambiguous_colon = "s:path:with:colons?q=1:2"_span; // 路径中的多个冒号
    constexpr auto empty_components = "s:? # "_span; // 空hier-part+查询+片段

    static_assert(URI(tricky_encoding));
    static_assert(URI(ambiguous_colon));
    static_assert(URI("s:?#"_span));
    static_assert(not URI(empty_components));

    // 非ASCII测试（需UTF-8处理）//NOTE: 网络流传输的只有ASCII
    // constexpr auto unicode_uri =
    //     "http://例子.测试/路径?查询=值#片段"_span; // 需预处理为UTF-8编码
    //                                                // 注意：需在实际代码中进行URL编码

    return 0;
}

// NOLINTEND