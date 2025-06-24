#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

// NOLINTBEGIN

struct wildcard
{
    constexpr explicit wildcard(std::string_view input) noexcept
    {
        // 步骤1: 编译期修剪输入字符串
        constexpr auto trim = [](std::string_view s) constexpr noexcept {
            // 移除前导空白
            while (!s.empty() && is_whitespace(s.front()))
            {
                s.remove_prefix(1);
            }
            // 移除尾部空白
            while (!s.empty() && is_whitespace(s.back()))
            {
                s.remove_suffix(1);
            }
            return s;
        };

        std::string_view trimmed = trim(input);

        // 步骤2: 查找冒号位置
        size_t colon_pos = 0;
        while (colon_pos < trimmed.size() && trimmed[colon_pos] != ':')
        {
            ++colon_pos;
        }

        // 步骤3: 分割并修剪name和type
        if (colon_pos < trimmed.size())
        {
            name_ = trim(trimmed.substr(0, colon_pos));
            type_ = trim(trimmed.substr(colon_pos + 1));
        }

        assert(name_ != "");
        assert(type_ != "");
    }

    constexpr const auto &name() const noexcept
    {
        return name_;
    }
    constexpr const auto &type() const noexcept
    {
        return type_;
    }

  private:
    // 自定义constexpr的字符判断函数
    static constexpr bool is_whitespace(char c) noexcept
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    }
    std::string_view name_;
    std::string_view type_;
};

struct path_mata
{
    constexpr explicit path_mata(std::string_view s) noexcept
    {
        assert(s.size() != 0);
        auto const k_size = s.size();
        if (k_size > 2 && s[0] == '{' && s[k_size - 1] == '}')
        {
            value_.emplace<wildcard>(s.substr(1, s.size() - 2));
        }
        else
            value_.emplace<std::string_view>(s);
    }

    [[nodiscard]] constexpr const auto &value() const noexcept
    {
        return value_;
    }

    [[nodiscard]] constexpr bool normal_path() const noexcept
    {
        return value_.index() == 0;
    }

    constexpr const auto &name() const noexcept
    {
        assert(not normal_path());
        return std::get<1>(value_).name();
    }
    constexpr const auto &type() const noexcept
    {
        assert(not normal_path());
        return std::get<1>(value_).type();
    }

    constexpr bool operator==(const std::string_view &s) const noexcept
    {
        if (not normal_path())
            return true;
        return s == std::get<0>(value_);
    }
    constexpr bool operator==(const std::string &s) const noexcept
    {
        return operator==(std::string_view(s));
    }
    friend constexpr bool operator==(const path_mata &a, const path_mata &b) noexcept
    {
        return a.normal_path() && b.normal_path()
                   ? std::get<0>(a.value_) == std::get<0>(b.value_)
                   : true;
    }

  private:
    std::variant<std::string_view, wildcard> value_;
};

struct http_path
{
    template <size_t N>
    constexpr http_path(const char (&str)[N]) noexcept : http_path(std::string_view(str))
    {
    }

    constexpr explicit http_path(std::string_view s) noexcept
    {
        // NOTE: '/' 需要特殊处理
        //  /a/b/c => a,b,c
        constexpr auto k_split_char = '/';
        assert(s.size() > 0 && s[0] == k_split_char);
        auto const k_size = s.size();
        if (k_size == 1)
        {
            path_.reserve(1);
            path_.emplace_back(std::string_view{"/"});
        }
        else
        {
            size_t start = 0;
            std::vector<path_mata> tmp;
            while (true)
            {
                auto end = s.find(k_split_char, start + 1);
                if (end < k_size && end - start > 1)
                {
                    tmp.emplace_back(s.substr(start + 1, end - start - 1));
                    start = end;
                    continue;
                }
                if (start + 1 < k_size)
                    tmp.emplace_back(s.substr(start + 1, k_size - 1));
                break;
            }
            path_ = std::move(tmp);
            path_.shrink_to_fit();
        }
    }

    constexpr const auto &path() const noexcept
    {
        return path_;
    }

    friend constexpr bool operator==(const http_path &a, const http_path &b) noexcept
    {
        if (auto const k_size = a.path_.size(); k_size == b.path_.size())
        {
            for (size_t i{0}; i < k_size; ++i)
            {
                if (a.path_[i] != b.path_[i])
                    return false;
            }
            return true;
        }
        return false;
    }

  private:
    std::vector<path_mata> path_;
};

struct request_meta
{
    std::string_view method;
    http_path request_target;
    std::string_view version;
    constexpr bool operator==(const request_meta &) const = default; // since C++20
    struct request_meta_hash
    {
        std::size_t operator()(const request_meta &s) const noexcept
        {
            // 计算method的哈希
            std::size_t h1 = std::hash<std::string_view>{}(s.method);

            // 计算http_path的哈希（需要为path_mata和http_path实现哈希）
            std::size_t h2 = 0;
            for (const auto &segment : s.request_target.path())
            {
                // 关键修改：统一通配符和具体段的哈希计算
                std::size_t seg_hash;
                if (segment.normal_path())
                    seg_hash =
                        std::hash<std::string_view>{}(std::get<0>(segment.value()));
                else
                    seg_hash = std::hash<std::string_view>{}("*");

                h2 ^= seg_hash;
                // 旋转哈希位
                h2 = (h2 << 1) | (h2 >> (sizeof(std::size_t) * 8 - 1));
            }

            // 计算version的哈希
            std::size_t h3 = std::hash<std::string_view>{}(s.version);

            // 合并哈希（可以使用更复杂的组合方式）
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
};
class Router
{
    struct RouteKey
    {
        std::string_view method;
        size_t segment_count;
        std::string_view version;

        bool operator==(const RouteKey &other) const
        {
            return method == other.method && segment_count == other.segment_count &&
                   version == other.version;
        }
    };

    struct RouteKeyHash
    {
        size_t operator()(const RouteKey &key) const noexcept
        {
            size_t h1 = std::hash<std::string_view>{}(key.method);
            size_t h2 = std::hash<size_t>{}(key.segment_count);
            size_t h3 = std::hash<std::string_view>{}(key.version);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    using RouteMap = std::unordered_map<RouteKey, std::vector<std::pair<http_path, int>>,
                                        RouteKeyHash>;
    RouteMap routes_;

  public:
    void add_route(const request_meta &meta, int value)
    {
        RouteKey key{meta.method, meta.request_target.path().size(), meta.version};

        routes_[key].emplace_back(meta.request_target, value);
    }

    int find_route(const request_meta &meta) const
    {
        RouteKey key{meta.method, meta.request_target.path().size(), meta.version};

        auto it = routes_.find(key);
        if (it == routes_.end())
            return -1;

        // 在匹配的分组中线性搜索
        for (const auto &[path, value] : it->second)
        {
            if (path == meta.request_target)
            {
                return value;
            }
        }

        return -1;
    }

    bool contains(const request_meta &meta) const
    {
        return find_route(meta) != -1;
    }
};

int main()
{

    // 使用新的路由系统
    {
        Router router;

        // 添加路由
        router.add_route(request_meta{"GET", "/", "HTTP/1.1"}, 1);
        router.add_route(request_meta{"GET", "/user/{id: int}", "HTTP/1.1"}, 2);
        router.add_route(request_meta{"GET", "/product/{name: string}", "HTTP/1.1"}, 3);

        // 测试路由匹配
        assert(router.contains(request_meta{"GET", "/", "HTTP/1.1"}));
        assert(router.find_route(request_meta{"GET", "/", "HTTP/1.1"}) == 1);

        assert(router.contains(request_meta{"GET", "/user/123", "HTTP/1.1"}));
        assert(router.find_route(request_meta{"GET", "/user/123", "HTTP/1.1"}) == 2);

        assert(router.contains(request_meta{"GET", "/product/widget", "HTTP/1.1"}));
        assert(router.find_route(request_meta{"GET", "/product/widget", "HTTP/1.1"}) ==
               3);
        assert(router.find_route(request_meta{"GET", "/product/widget2", "HTTP/1.1"}) ==
               3);

        // 测试不匹配的情况
        assert(!router.contains(request_meta{"POST", "/", "HTTP/1.1"}));
        assert(!router.contains(request_meta{"GET", "/user", "HTTP/1.1"}));
        assert(!router.contains(request_meta{"GET", "/user/123/profile", "HTTP/1.1"}));
    }

    std::cout << "All tests passed!\n";
    return 0;
}
// NOLINTEND