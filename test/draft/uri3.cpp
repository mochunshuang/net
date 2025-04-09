#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <optional>
#include <regex>
#include <expected>
#include <cstdint>
#include <cctype>
#include <unordered_map>

// https://github.com/boostorg/url

/**
 * @brief https://www.rfc-editor.org/rfc/rfc3986.html

3.  Syntax Components
  The generic URI syntax consists of a hierarchical sequence of
   components referred to as the scheme, authority, path, query, and
   fragment.

      URI         = scheme ":" hier-part [ "?" query ] [ "#" fragment ]

      hier-part   = "//" authority path-abempty
                  / path-absolute
                  / path-rootless
                  / path-empty

    for some case:
        foo://example.com:8042/over/there?name=ferret#nose
        \_/   \______________/\_________/ \_________/ \__/
        |           |            |            |        |
    scheme     authority       path        query   fragment
        |   _____________________|__
        / \ /                        \
        urn:example:animal:ferret:nose

3.1.  Scheme
 scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )

3.2.  Authority
authority   = [ userinfo "@" ] host [ ":" port ]

    3.2.1.  User Information
    userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
    3.2.2.  Host
    host        = IP-literal / IPv4address / reg-name
    NOTE:
        IP-literal = "[" ( IPv6address / IPvFuture  ) "]"
        IPvFuture  = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
        IPv6address =                          6( h16 ":" ) ls32
                  /                       "::" 5( h16 ":" ) ls32
                  / [               h16 ] "::" 4( h16 ":" ) ls32
                  / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
                  / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
                  / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
                  / [ *4( h16 ":" ) h16 ] "::"              ls32
                  / [ *5( h16 ":" ) h16 ] "::"              h16
                  / [ *6( h16 ":" ) h16 ] "::"

        ls32        = ( h16 ":" h16 ) / IPv4address
                    ; least-significant 32 bits of address

        h16         = 1*4HEXDIG
                    ; 16 bits of address represented in hexadecimal
        ....
    3.2.3.  Port
        port        = *DIGIT
    3.3.  Path
          path      = path-abempty    ; begins with "/" or is empty
                    / path-absolute   ; begins with "/" but not "//"
                    / path-noscheme   ; begins with a non-colon segment
                    / path-rootless   ; begins with a segment
                    / path-empty      ; zero characters

 */

struct Uri
{
    enum class ParseError : uint8_t
    {
        INVALID_FORMAT,
        INVALID_SCHEME,
        INVALID_AUTHORITY,
        INVALID_PORT,
        INVALID_HOST
    };

    struct Authority
    {
        using port_type = uint16_t;
        std::string userinfo;
        std::string host;
        std::optional<port_type> port;

        // authority   = [ userinfo "@" ] host [ ":" port ]
        static std::expected<Authority, ParseError> parse(
            std::string_view auth_str) noexcept
        {
            Authority auth;
            std::string_view remaining = auth_str;

            // Split userinfo and hostport
            if (auto at_pos = remaining.rfind('@'); at_pos != std::string_view::npos)
            {
                auth.userinfo = remaining.substr(0, at_pos);
                remaining = remaining.substr(at_pos + 1);
            }

            // Handle IPv6 literal
            bool is_ipv6 = !remaining.empty() && remaining.front() == '[';
            size_t port_pos = std::string_view::npos;

            if (is_ipv6)
            {
                auto close_bracket = remaining.find(']');
                if (close_bracket == std::string_view::npos)
                {
                    return std::unexpected(ParseError::INVALID_HOST);
                }
                port_pos = remaining.find(':', close_bracket);
            }
            else
            {
                port_pos = remaining.rfind(':');
            }

            // Extract port
            if (port_pos != std::string_view::npos)
            {
                std::string_view port_str = remaining.substr(port_pos + 1);
                if (port_str.empty())
                {
                    return std::unexpected(ParseError::INVALID_PORT);
                }

                try
                {
                    size_t pos = 0;
                    auto port = std::stoi(std::string(port_str), &pos);
                    if (pos != port_str.size() || port < 0 ||
                        port > std::numeric_limits<port_type>::max())
                    {
                        return std::unexpected(ParseError::INVALID_PORT);
                    }
                    auth.port = static_cast<port_type>(port);
                }
                catch (...)
                {
                    return std::unexpected(ParseError::INVALID_PORT);
                }
                remaining = remaining.substr(0, port_pos);
            }

            // Process host
            auth.host = remaining;
            if (is_ipv6)
            {
                if (auth.host.size() < 2 || auth.host.back() != ']')
                {
                    return std::unexpected(ParseError::INVALID_HOST);
                }
                auth.host = auth.host.substr(1, auth.host.size() - 2);
                if (!is_valid_ipv6(auth.host))
                {
                    return std::unexpected(ParseError::INVALID_HOST);
                }
            }
            else if (!is_valid_host(auth.host))
            {
                return std::unexpected(ParseError::INVALID_HOST);
            }

            return auth;
        }

      private:
        static bool is_valid_host(std::string_view host) noexcept // NOLINT
        {
            // Simplified host validation (RFC 3986 section 3.2.2)
            return !host.empty() && std::ranges::all_of(host, [](char c) {
                return std::isalnum(c) || c == '.' || c == '-' || c == '_' || c == '~' ||
                       c == '%' || c == '!' || c == '$' || c == '&' || c == '\'' ||
                       c == '(' || c == ')' || c == '*' || c == '+' || c == ',' ||
                       c == ';' || c == '=' || c == ':' || c == '@';
            });
        }

        static bool is_valid_ipv6(std::string_view ipv6) noexcept // NOLINT
        {
            // Simplified IPv6 validation
            size_t colon_count = std::count(ipv6.begin(), ipv6.end(), ':');
            return colon_count >= 2 && colon_count <= 8; // NOLINT
        }
    };

    std::string scheme;
    std::optional<Authority> authority;
    std::string path;
    std::string query;
    std::string fragment;

    static std::expected<Uri, ParseError> parse(std::string_view uri_str) noexcept
    {
        // RFC 3986 Appendix B regex
        static constexpr std::string_view URI_REGEX = // NOLINT
            R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)";

        std::match_results<std::string_view::const_iterator> match;
        if (!std::regex_match(uri_str.begin(), uri_str.end(), match,
                              std::regex(URI_REGEX.data(), URI_REGEX.size())))
        {
            return std::unexpected(ParseError::INVALID_FORMAT);
        }

        Uri uri;
        /**
        scheme    = $2
        authority = $4
        path      = $5
        query     = $7
        fragment  = $9
        */
        constexpr auto k_scheme_index = 2;
        constexpr auto k_authority_index = 4;
        constexpr auto k_path_index = 5;
        constexpr auto k_query_index = 7;
        constexpr auto k_fragment_index = 9;

        // Scheme validation
        if (match[k_scheme_index].matched)
        {
            uri.scheme = match[k_scheme_index].str();
            if (not std::regex_match(uri.scheme, std::regex("^[a-zA-Z][a-zA-Z0-9+.-]*$")))
            {
                return std::unexpected(ParseError::INVALID_SCHEME);
            }
        }

        // Authority parsing
        if (match[k_authority_index].matched)
        {
            auto auth = Authority::parse(match[k_authority_index].str());
            if (!auth)
                return std::unexpected(auth.error());
            uri.authority = *auth;
        }

        // Path component
        uri.path = match[k_path_index].str();

        // Query component
        if (match[k_query_index].matched)
            uri.query = match[k_query_index].str();

        // Fragment component
        if (match[k_fragment_index].matched)
            uri.fragment = match[k_fragment_index].str();

        // 新增默认端口处理
        if (uri.authority.has_value() && !uri.authority->port)
        {
            static const std::unordered_map<std::string, Authority::port_type>
                DEFAULT_PORTS = {// NOLINT
                                 {"http", 80},
                                 {"https", 443},
                                 {"ftp", 21},
                                 {"ssh", 22},
                                 {"smtp", 25}};

            if (auto it = DEFAULT_PORTS.find(uri.scheme); it != DEFAULT_PORTS.end())
            {
                uri.authority->port = it->second;
            }
        }
        return uri;
    }
};

// 测试用例
void test_typical_cases() // NOLINT
{
    // 标准HTTP地址
    auto http = Uri::parse("http://www.example.com:80/path?query#frag");
    assert(http.has_value());
    assert(http->scheme == "http");
    assert(http->authority->host == "www.example.com");
    assert(http->authority->port == 80);
    assert(http->path == "/path");

    // IPv6地址
    auto ipv6 = Uri::parse("https://[2001:db8::1]:443/");
    assert(ipv6.has_value());
    assert(ipv6->authority->host == "2001:db8::1");
    assert(ipv6->authority->port == 443);

    // 用户信息包含特殊字符
    auto auth = Uri::parse("ftp://user%40:pass@host.com");
    assert(auth.has_value());
    assert(auth->authority->userinfo == "user%40:pass");

    // URN格式
    auto urn = Uri::parse("urn:isbn:0451450523");
    assert(urn.has_value());
    assert(urn->scheme == "urn");
    assert(urn->path == "isbn:0451450523");
}

void test_edge_cases() // NOLINT
{
    // 空端口
    auto empty_port = Uri::parse("http://host.com:");
    assert(empty_port.error() == Uri::ParseError::INVALID_PORT);

    // 非法IPv6
    auto bad_ipv6 = Uri::parse("http://[::1");
    assert(bad_ipv6.error() == Uri::ParseError::INVALID_HOST);

    // 非法scheme
    auto bad_scheme = Uri::parse("123scheme://host");
    assert(bad_scheme.error() == Uri::ParseError::INVALID_SCHEME);

    // 最大合法端口
    auto max_port = Uri::parse("http://host:65535");
    assert(max_port->authority->port == 65535);

    // 超出范围端口
    auto overflow_port = Uri::parse("http://host:65536");
    assert(overflow_port.error() == Uri::ParseError::INVALID_PORT);
}

void test_query_fragment() // NOLINT
{
    // 空查询和片段
    auto empty = Uri::parse("mailto:user@domain.com?#");
    assert(empty->query.empty());
    assert(empty->fragment.empty());

    // 特殊字符
    auto special = Uri::parse("http://host/?query=@&test#frag#ment");
    assert(special->query == "query=@&test");
    assert(special->fragment == "frag#ment");
}

// 新增测试用例
void test_default_ports() // NOLINT
{
    // HTTP默认端口
    auto http = Uri::parse("http://example.com/path");
    assert(http.has_value());
    assert(http->authority->port == 80);

    // HTTPS默认端口
    auto https = Uri::parse("https://example.com/path");
    assert(https.has_value());
    assert(https->authority->port == 443);

    // 显式指定默认端口
    auto explicit_http = Uri::parse("http://example.com:80/path");
    assert(explicit_http.has_value());
    assert(explicit_http->authority->port == 80);

    // 非标准Scheme不设置默认端口
    auto custom = Uri::parse("myapp://example.com/path");
    assert(custom.has_value());
    assert(not custom->authority->port.has_value());
}

void test_comprehensive_uri_parsing() // NOLINT
{
    // 多参数查询测试
    {
        auto uri = Uri::parse("http://example.com/path?name=John&age=30&lang=en");
        assert(uri.has_value());
        assert(uri->query == "name=John&age=30&lang=en");
        assert(uri->path == "/path");
    }

    // 同名多参数测试
    {
        auto uri =
            Uri::parse("https://shop.com/search?category=books&category=electronics");
        assert(uri.has_value());
        assert(uri->query == "category=books&category=electronics");
    }

    // 特殊字符查询测试
    {
        auto uri = Uri::parse("http://api.com/?filter=price%3E100&sort=desc&view=grid");
        assert(uri.has_value());
        assert(uri->query == "filter=price%3E100&sort=desc&view=grid");
    }

    // 空值参数测试
    {
        auto uri = Uri::parse("ftp://files.com/?download&verify");
        assert(uri.has_value());
        assert(uri->query == "download&verify");
    }

    // 混合组件测试
    {
        auto uri = Uri::parse(
            "https://user:pass@[::1]:8080/docs;version=3?q=regex&debug=true#page=2");
        assert(uri.has_value());
        assert(uri->scheme == "https");
        assert(uri->authority->userinfo == "user:pass");
        assert(uri->authority->host == "::1");
        assert(uri->authority->port == 8080);
        assert(uri->path == "/docs;version=3");
        assert(uri->query == "q=regex&debug=true");
        assert(uri->fragment == "page=2");
    }

    // 边界条件测试
    {
        // 空查询
        auto empty_query = Uri::parse("http://test.com/?");
        assert(empty_query->query.empty());

        // 最大长度端口
        auto max_port = Uri::parse("http://host:65535");
        assert(max_port->authority->port == 65535);

        // 混合编码字符
        auto encoded = Uri::parse("http://xn--bcher-kva.ch/?city=Z%C3%BCrich");
        assert(encoded->query == "city=Z%C3%BCrich");
    }

    // 错误处理测试
    {
        // 无效端口值
        auto invalid_port = Uri::parse("http://host:99999");
        assert(not invalid_port.has_value());
        assert(invalid_port.error() == Uri::ParseError::INVALID_PORT);

        // 不完整IPv6地址
        auto bad_ipv6 = Uri::parse("http://[::ffff");
        assert(not bad_ipv6.has_value());
        assert(bad_ipv6.error() == Uri::ParseError::INVALID_HOST);
    }

    // 默认端口处理验证
    {
        // HTTP默认端口
        auto http_default = Uri::parse("http://example.com");
        assert(http_default.has_value());
        assert(http_default->authority->port == 80);

        // HTTPS显式指定默认端口
        auto https_explicit = Uri::parse("https://secure.com:443");
        assert(https_explicit.has_value());
        assert(https_explicit->authority->port == 443);
    }
}

int main()
{
    static_assert(std::numeric_limits<uint16_t>::max() == 65535); // NOLINT
    test_typical_cases();
    test_edge_cases();
    test_query_fragment();
    test_default_ports();
    test_comprehensive_uri_parsing();
    std::cout << "All tests passed!\n";
    return 0;
}