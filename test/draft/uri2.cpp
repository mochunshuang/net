#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <stdexcept>
#include <optional>
#include <format>
#include <cctype>
#include <sstream>
#include <iomanip>
#include <map>

class URI
{
  public:
    // ------------------- Builder 模式 -------------------
    class Builder
    {
      public:
        Builder &set_scheme(std::string_view scheme)
        {
            scheme_ = scheme;
            return *this;
        }

        Builder &set_authority(std::string_view authority)
        {
            authority_ = authority;
            return *this;
        }

        Builder &set_path(std::string_view path)
        {
            path_ = path;
            return *this;
        }

        Builder &set_query(std::string_view query)
        {
            query_ = query;
            return *this;
        }

        Builder &set_fragment(std::string_view fragment)
        {
            fragment_ = fragment;
            return *this;
        }

        URI build() const
        {
            return URI(scheme_, authority_, path_, query_, fragment_);
        }

      private:
        std::string scheme_;
        std::string authority_;
        std::string path_;
        std::string query_;
        std::string fragment_;
    };

    // ------------------- 构造函数 -------------------
    explicit URI(std::string_view uri_str)
    {
        if (!parse(uri_str))
        {
            throw std::invalid_argument("Invalid URI format");
        }
        apply_protocol_specific_rules();
    }

    URI(std::string scheme, std::string authority, std::string path, std::string query,
        std::string fragment)
        : scheme_(std::move(scheme)), authority_(std::move(authority)),
          path_(std::move(path)), query_(std::move(query)), fragment_(std::move(fragment))
    {
        apply_protocol_specific_rules();
    }

    // ------------------- 组件访问 -------------------
    std::string_view scheme() const
    {
        return scheme_;
    }
    std::string_view authority() const
    {
        return authority_;
    }
    std::string_view path() const
    {
        return path_;
    }
    std::string_view query() const
    {
        return query_;
    }
    std::string_view fragment() const
    {
        return fragment_;
    }

    // ------------------- URL 编码/解码 -------------------
    static std::string encode(std::string_view str)
    {
        std::ostringstream escaped;
        for (char c : str)
        {
            if (is_unreserved_char(c))
            {
                escaped << c;
            }
            else
            {
                escaped << '%' << std::hex << std::uppercase << (int)(unsigned char)c;
            }
        }
        return escaped.str();
    }

    static std::string decode(std::string_view str)
    {
        std::ostringstream unescaped;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if (str[i] == '%' && i + 2 < str.size())
            {
                int hex_val;
                std::istringstream hex_stream(std::string(str.substr(i + 1, 2)));
                if (hex_stream >> std::hex >> hex_val)
                {
                    unescaped << static_cast<char>(hex_val);
                    i += 2;
                }
                else
                {
                    unescaped << str[i];
                }
            }
            else
            {
                unescaped << str[i];
            }
        }
        return unescaped.str();
    }

    // ------------------- 生成完整 URI -------------------
    std::string to_string() const
    {
        std::string result;
        if (!scheme_.empty())
            result += std::format("{}:", scheme_);
        if (!authority_.empty())
            result += std::format("//{}", authority_);
        result += path_;
        if (!query_.empty())
            result += std::format("?{}", query_);
        if (!fragment_.empty())
            result += std::format("#{}", fragment_);
        return result;
    }

  private:
    std::string scheme_;
    std::string authority_;
    std::string path_;
    std::string query_;
    std::string fragment_;

    // ------------------- RFC 3986 正则表达式 -------------------
    static constexpr std::string_view URI_REGEX =
        R"(^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\?([^#]*))?(#(.*))?)";

    // ------------------- 协议特定规则 -------------------
    void apply_protocol_specific_rules()
    {
        // 规则 1: HTTP/HTTPS 默认端口省略
        if ((scheme_ == "http" || scheme_ == "https") && !authority_.empty())
        {
            size_t port_pos = authority_.rfind(':');
            if (port_pos != std::string::npos)
            {
                std::string_view port_str =
                    std::string_view(authority_).substr(port_pos + 1);
                if ((scheme_ == "http" && port_str == "80") ||
                    (scheme_ == "https" && port_str == "443"))
                {
                    authority_.resize(port_pos);
                }
            }
        }

        // 规则 2: 如果 authority 非空但 path 为空，path 设为 "/"
        if (!authority_.empty() && path_.empty())
        {
            path_ = "/";
        }
    }

    // ------------------- URI 解析 -------------------
    bool parse(std::string_view uri_str)
    {
        std::regex re(URI_REGEX.data());
        std::cmatch match;
        if (!std::regex_match(uri_str.data(), match, re))
        {
            return false;
        }

        scheme_ = match[2].str();
        authority_ = match[4].str();
        path_ = match[5].str();
        query_ = decode(match[7].str());    // 自动解码 query
        fragment_ = decode(match[9].str()); // 自动解码 fragment

        return true;
    }

    // ------------------- URL 编码工具函数 -------------------
    static bool is_unreserved_char(char c)
    {
        return std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~';
    }
};

// ------------------- 测试代码 -------------------
int main()
{
    try
    {
        // 测试 1: Builder 模式
        URI uri1 = URI::Builder()
                       .set_scheme("https")
                       .set_authority("example.com:443")
                       .set_path("/search")
                       .set_query("q=面向对象+设计")
                       .set_fragment("results")
                       .build();
        std::cout << "[Builder 模式]\n" << uri1.to_string() << "\n\n";

        // 测试 2: URL 编码/解码
        std::string encoded_query = URI::encode("name=张三&age=20");
        std::cout << "[URL 编码]\n原始: name=张三&age=20\n编码后: " << encoded_query
                  << "\n";
        std::cout << "解码后: " << URI::decode(encoded_query) << "\n\n";

        // 测试 3: 协议特定逻辑 (HTTPS 默认端口省略)
        URI uri3("https://example.com:443/");
        std::cout << "[协议逻辑: HTTPS 端口省略]\n" << uri3.to_string() << "\n";

        // 测试 4: 复杂 URI 解析
        URI uri4("urn:example:animal:ferret?type=白色#nose");
        std::cout << "\n[复杂 URN 解析]\nScheme: " << uri4.scheme()
                  << "\nPath: " << uri4.path() << "\nQuery: " << uri4.query()
                  << "\nFragment: " << uri4.fragment() << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}