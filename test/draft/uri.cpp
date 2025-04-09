#include <iostream>
#include <regex>
#include <string_view>
#include <vector>
#include <format> // C++20/23 格式化库

// NOLINTBEGIN

/**
   The following are two example URIs and their component parts:

         foo://example.com:8042/over/there?name=ferret#nose
         \_/   \______________/\_________/ \_________/ \__/
          |           |            |            |        |
       scheme     authority       path        query   fragment
          |   _____________________|__
         / \ /                        \
         urn:example:animal:ferret:nose
 */

// RFC 3986 URI 正则表达式
constexpr std::string_view URI_REGEX =
    R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)";

// 测试用例结构体
struct URITestCase
{
    std::string_view uri;
    std::string_view scheme;
    std::string_view authority;
    std::string_view path;
    std::string_view query;
    std::string_view fragment;
    bool should_match;
};

// 测试用例集合
const std::vector<URITestCase> TEST_CASES = {
    // 标准 HTTP URI
    {"https://example.com:80/path?key=value#footer", "https", "example.com:80", "/path",
     "key=value", "footer", true},
    // 无 authority 的 URI (如 mailto)
    {
        "mailto:user@example.com", "mailto", "", "", "", "", true // authority 为空
    },
    // 相对 URI (无 scheme)
    {"//example.com/path", "", "example.com", "/path", "", "", true},
    // 仅 path
    {"/path/to/resource", "", "", "/path/to/resource", "", "", true},
    // 带 query 和 fragment
    {"/search?q=regex#results", "", "", "/search", "q=regex", "results", true},
    // 非法 URI (包含空格)
    {
        "http://example.com/ invalid", "", "", "", "", "", false // 不应匹配
    },
    // 空字符串
    {"", "", "", "", "", "", false},
    // 复杂 authority (含用户名和端口)
    {"ftp://user@host:21/files", "ftp", "user@host:21", "/files", "", "", true}};

// 运行测试
int main()
{
    std::regex uri_regex(URI_REGEX.data());
    size_t passed = 0;

    for (const auto &test : TEST_CASES)
    {
        std::cmatch match;
        bool matched = std::regex_match(test.uri.data(), match, uri_regex);

        if (matched != test.should_match)
        {
            std::cout << std::format("[FAIL] URI: {}\n", test.uri);
            continue;
        }

        if (!test.should_match)
        {
            std::cout << std::format("[PASS] (Expected fail) URI: {}\n", test.uri);
            passed++;
            continue;
        }

        // 验证捕获组
        bool is_valid =
            ((match[2].str() == test.scheme) && (match[4].str() == test.authority) &&
             (match[5].str() == test.path) && (match[7].str() == test.query) &&
             (match[9].str() == test.fragment));

        if (is_valid)
        {
            std::cout << std::format("[PASS] URI: {}\n", test.uri);
            passed++;
        }
        else
        {
            std::cout << std::format(
                "[FAIL] URI: {}\n  Expected: scheme={}, authority={}, path={}, query={}, "
                "fragment={}\n  Actual: scheme={}, authority={}, path={}, query={}, "
                "fragment={}\n",
                test.uri, test.scheme, test.authority, test.path, test.query,
                test.fragment, match[2].str(), match[4].str(), match[5].str(),
                match[7].str(), match[9].str());
        }
    }

    std::cout << std::format("\nResults: {}/{} passed\n", passed, TEST_CASES.size());
    return (passed == TEST_CASES.size()) ? 0 : 1;
}
// NOLINTEND