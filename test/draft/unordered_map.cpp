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
                {
                    seg_hash =
                        std::hash<std::string_view>{}(std::get<0>(segment.value()));
                }
                else
                {
                    seg_hash = std::hash<std::string_view>{}("*");
                }

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
int main()
{
    // NOTE: GET /abc?a=b HTTP/1.1 才是浏览器给出的报文
    {
        std::vector<std::string_view> vec = {std::string_view{"a/b/c"}};
        {
            assert(vec[0].size() == 5);
            assert(vec[0][5 - 1] == 'c');
        }

        assert(vec.size() == 1);
        assert(vec[0] == std::string("a/b/c"));
        {
            auto v = vec;
            assert(v[0] == std::string("a/b/c"));
            constexpr auto *s = "a/c/d"; // NOLINT
            v.emplace_back(s);
            assert(v[1] == std::string("a/c/d"));
        }
    }
    {
        constexpr std::string_view path{"{a: int}"};
        constexpr std::string_view path0{"{a : int}"};
        constexpr std::string_view path1{"{ a : int }"};

        constexpr wildcard w1(path.substr(1, path.size() - 2)); // 去掉大括号
        constexpr wildcard w2(path0.substr(1, path0.size() - 2));
        constexpr wildcard w3(path1.substr(1, path1.size() - 2));

        static_assert(w1.name() == "a" && w1.type() == "int");
        static_assert(w2.name() == "a" && w2.type() == "int");
        static_assert(w3.name() == "a" && w3.type() == "int");
    }
    {
        constexpr std::string_view path{"{}"};
        // constexpr wildcard w1(path.substr(1, path.size() - 2)); // 去掉大括号
        // static_assert(w1.name() == "" && w1.type() == "");
    }

    // http_path
    {
        http_path path{"/user/abc"};
        assert(path.path().size() == 2);

        assert(path.path()[0] == std::string_view("user"));
        assert(path.path()[1] == std::string_view("abc"));

        {
            http_path path{"/user/abc/"};
            assert(path.path().size() == 2);
            assert(path.path()[0] == std::string_view("user"));
            assert(path.path()[1] == std::string_view("abc"));
        }

        {
            constexpr path_mata p{"asd"};
            constexpr path_mata p1{"asd"};
            static_assert(p == p1);
            static_assert(p == path_mata{"{a:int}"});
            static_assert(p == path_mata{"{a : int}"});
            static_assert(p.normal_path());

            static_assert(not path_mata{"{a : int}"}.normal_path());
            static_assert(path_mata{"{a: int}"}.name() == "a");
            static_assert(path_mata{"{a:int}"}.type() == "int");
        }
        {
            http_path path{"/user/abc"};
            http_path path1{"/user/abc"};
            assert(path == path1);

            http_path path2{"/user/{a: string}"};
            assert(path == path2);
            http_path path3{"/user/{a: string}/"};
            assert(path == path3);

            http_path path4{"/user/{a: string}/id"};
            assert(path != path4);

            http_path path5{"/user/bcd"};
            assert(path != path5);
            http_path path6{"/users/abc"};
            assert(path != path6);

            {
                http_path path1{"/user/{a: string}"};
                http_path path2{"/user/{abc: string}"};
                http_path path3{"/user/{abc: id}"};
                assert(path1 == path2);
                assert(path1 == path3);
            }
        }
    }

    // request_meta
    {
        std::unordered_map<request_meta, int, request_meta::request_meta_hash> map;

        map.insert({request_meta{"GET", "/", "HTTP/1.1"}, 1});
        assert(map.size() == 1);

        auto v = map[request_meta{"GET", "/", "HTTP/1.1"}];
        assert(v == 1);
        map.at(request_meta{"GET", "/", "HTTP/1.1"});
        assert(map.at(request_meta{"GET", "/", "HTTP/1.1"}) == 1);

        assert(map.contains(request_meta{"GET", "/", "HTTP/1.1"}));
        // 继续
        map.insert({request_meta{"GET", "/user/{id: int}", "HTTP/1.1"}, 1});

        // NOTE: hash 是失败的。hash 值无法 两个来计算
        //  assert(map.contains(request_meta{"GET", "/user/123", "HTTP/1.1"}));
    }

    std::cout << "main done\n";
    return 0;
}
// NOLINTEND