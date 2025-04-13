

#include "../test_common/test_macro.hpp"
#include "../../include/net.hpp"
#include <cassert>

// NOLINTBEGIN

using OCTET = std::uint8_t;

int main()
{

    using namespace mcs::ABNF;
    using namespace mcs::ABNF::URI; // NOLINT

    //================= URI-reference 有效用例 =================
    // 完整URI形式
    constexpr auto full_uri = "https://user@example.com:8080/path?q=1#frag"_span;
    // 纯relative-ref形式（无scheme）
    constexpr auto rel_ref_full = "//example.com/doc#ch1"_span;
    // 混合编码测试
    constexpr auto encoded_uri_ref = "scheme:/a%20b?%23=%25#%2F"_span;

    // URI类型边界
    constexpr auto minimal_uri = "a:b"_span;              // 最小合法URI
    constexpr auto urn_like = "urn:isbn:0451450523"_span; // URN形式
    constexpr auto uri_with_empty = "s:?#"_span;          // 空hier+空query+空fragment

    // relative-ref类型边界
    constexpr auto empty_ref = ""_span;                   // 空字符串（path-empty）
    constexpr auto root_relative = "/search?q=test"_span; // 根相对路径
    constexpr auto noscheme_rel = "folder/file.txt"_span; // 无scheme路径
    constexpr auto frag_only = "#top"_span;               // 仅片段（合法relative-ref）

    static_assert(uri_reference(full_uri));
    static_assert(uri_reference(rel_ref_full));
    static_assert(uri_reference(encoded_uri_ref));
    static_assert(uri_reference(minimal_uri));
    static_assert(uri_reference(urn_like));
    static_assert(uri_reference(uri_with_empty));
    static_assert(uri_reference(empty_ref));
    static_assert(uri_reference(root_relative));
    static_assert(uri_reference(noscheme_rel));
    static_assert(uri_reference(frag_only));

    //================= URI-reference 无效用例 =================
    // 结构污染
    constexpr auto hybrid_ref = "http://host/../rel#frag"_span; // URI与relative-ref混合
    {
        // URI-reference = URI / relative-ref
        // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
        /**
          hier-part     = "//" authority path-abempty
                    / path-absolute
                    / path-rootless
                    / path-empty
        */
        static_assert(uri(hybrid_ref));
        {
            static_assert(scheme("http"_span));

            static_assert(authority("host"_span));       // hier-part: authority
            static_assert(path_abempty("/../rel"_span)); // hier-part: path_abempty

            static_assert(fragment("frag"_span));
        }
    }
    constexpr auto double_scheme = "http:https://host"_span; // 多重scheme
    constexpr auto colon_in_rel = "a/b:c/d"_span;            // relative-ref含非法冒号

    // 非法字符渗透
    constexpr auto space_in_scheme = "sch eme:/path"_span; // scheme含空格
    constexpr auto unencoded_space = "/a path#frag"_span;  // relative-ref路径未编码空格
    constexpr auto ctrl_char_query = "s:/?q=\x01"_span;    // 查询含控制字符

    // 组件格式错误
    constexpr auto bad_port_uri = "http://host:80a/path"_span; // 端口含字母
    constexpr auto rel_with_scheme = "http:path"_span;         // 伪relative-ref含scheme
    constexpr auto invalid_urn = "urn:-invalid"_span;          // URN路径格式错误

    // 编码错误
    constexpr auto malformed_pct = "s:/%X"_span;  // 无效百分比编码
    constexpr auto incomplete_pct = "s:/%A"_span; // 不完整百分比编码

    static_assert(uri_reference(hybrid_ref));
    static_assert(uri_reference(double_scheme));

    static_assert(uri_reference(colon_in_rel));

    static_assert(!uri_reference(space_in_scheme));
    static_assert(!uri_reference(unencoded_space));
    static_assert(!uri_reference(ctrl_char_query));
    static_assert(!uri_reference(bad_port_uri));

    static_assert(uri_reference(rel_with_scheme));
    {
        static_assert(uri(rel_with_scheme));
    }
    static_assert(uri_reference(invalid_urn));
    {
        static_assert(uri(invalid_urn));
    }

    static_assert(!uri_reference(malformed_pct));
    static_assert(!uri_reference(incomplete_pct));

    // 特殊说明用例
    constexpr auto tricky_uri =
        "http://example.com/../a/./b?q=.."_span; // 路径标准化前合法
    // constexpr auto max_length_ref = string_view(8000, 'a') + "#end"_span; // 超长引用
    constexpr auto ipv6_rel_ref = "//[2001:db8::1]/path"_span; // relative-ref含IPv6

    static_assert(uri_reference(tricky_uri));
    static_assert(uri_reference(ipv6_rel_ref));
    {
        static_assert(not uri(ipv6_rel_ref));
        // relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
        static_assert(relative_ref(ipv6_rel_ref));
        // relative-part = "//" authority path-abempty
        static_assert(relative_part(ipv6_rel_ref));
        {
            // authority     = [ userinfo "@" ] host [ ":" port ]
            static_assert(authority("[2001:db8::1]"_span));
            {
                //  host          = IP-literal / IPv4address / reg-name
                // IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
                static_assert(host("[2001:db8::1]"_span));
            }
        }
    }
    // static_assert(uri_reference(max_length_ref)); // 需运行时测试

    return 0;
}

// NOLINTEND