#include <algorithm>
#include <cassert>
#include <cstddef>
#include <limits>
#include <span>
#include <string_view>
#include <utility>

#include <iostream>
#include <vector>

std::pair<std::span<const char>, std::span<const char>> split_span(
    std::span<const char> s)
{
    auto it = std::ranges::find(s, '/');
    size_t pos = std::distance(s.begin(), it);
    if (it != s.end())
    {
        return {s.first(pos), s.subspan(pos)};
    }
    return {s, s.subspan(s.size())};
}

// 按第一个分隔符分割字符串，返回前后部分的 span 视图
// 按第一个分隔符分割 span，返回前后部分的视图
std::pair<std::span<const char>, std::span<const char>> split_first_span(
    std::span<const char> sp, char delimiter)
{
    for (size_t i = 0; i < sp.size(); ++i)
    {
        if (sp[i] == delimiter)
        {
            std::span<const char> first = sp.first(i);        // 前半部分（不含分隔符）
            std::span<const char> second = sp.subspan(i + 1); // 后半部分（跳过分隔符）
            return {first, second};
        }
    }
    // 未找到分隔符，返回整个 span 和空 span
    return {sp, std::span<const char>()};
}

int main()
{
    {
        // Empty span"
        std::string s;
        std::span<const char> sp{s.data(), s.size()};
        auto [first, second] = split_span(sp);
        assert(first.empty());
        assert(second.empty());
    }
    {
        //"No delimiter"
        std::string s = "no_delimiter";
        std::span<const char> sp{s.data(), s.size()};
        auto [first, second] = split_span(sp);
        assert(first.size() == s.size());
        assert(second.empty());
        assert(std::string(first.begin(), first.end()) == "no_delimiter");
    }
    {
        // Single delimiter in middle
        std::string s = "hello/world";
        std::span<const char> sp{s.data(), s.size()};
        auto [first, second] = split_span(sp);
        assert(std::string(first.begin(), first.end()) == "hello");
        assert(std::string(second.begin(), second.end()) == "/world");
    }
    {
        //"Delimiter at start"
        std::string s = "/start";
        std::span<const char> sp{s.data(), s.size()};
        auto [first, second] = split_span(sp);
        assert(first.empty());
        assert(std::string(second.begin(), second.end()) == "/start");
    }
    {
        //"Delimiter at end"
        std::string s = "end/";
        std::span<const char> sp{s.data(), s.size()};
        auto [first, second] = split_span(sp);
        assert(std::string(first.begin(), first.end()) == "end");
        assert(std::string(second.begin(), second.end()) == "/");
    }
    {
        // Multiple delimiters
        std::string s = "a/b/c";
        std::span<const char> sp{s.data(), s.size()};
        auto [first, second] = split_span(sp);
        assert(std::string(first.begin(), first.end()) == "a");
        assert(std::string(second.begin(), second.end()) == "/b/c");
    }
    {
        // NOTE:  detail
        std::string s = "hello/world";
        std::span<const char> sp{s};
        auto it = std::ranges::find(sp, '/');
        size_t idx = std::distance(sp.begin(), it);
        assert(idx == 5);

        auto s1 = sp.first(idx); // NOTE: 取出 idx 之前的
        assert(std::string(s1.begin(), s1.end()) == "hello");

        auto s2 = sp.subspan(idx + 1); // NOTE: 取出 idx 之后的
        assert(std::string(s2.begin(), s2.end()) == "world");
    }
    {
        // "hello/world/a/b/b/c/d"
        // => /world/a/b/b/c/d
        // => /world,/a,/b,/b,/c,/d
        {
            std::string s = "hello/world/a/b/c";
            std::span<const char> sp{s};

            // 分割第一个部分（直到第一个 '/'
            auto first_it = std::ranges::find(sp, '/');
            size_t first_idx = std::distance(sp.begin(), first_it);
            auto first_part = sp.first(first_idx);
            sp = sp.subspan(first_idx); // 剩余部分以 '/' 开头

            std::vector<std::string> parts;
            parts.emplace_back(first_part.begin(), first_part.end());

            while (!sp.empty())
            {
                // 从当前 '/' 的下一个位置开始查找
                auto tail = sp.subspan(1);
                auto next_it = std::ranges::find(tail, '/');

                if (next_it != tail.end())
                {
                    // 计算分割长度：当前 '/' 到下一个 '/' 的位置
                    size_t part_size = 1 + std::distance(tail.begin(), next_it);
                    auto part = sp.first(part_size);
                    parts.emplace_back(part.begin(), part.end());
                    sp = sp.subspan(part_size); // 移动剩余部分
                }
                else
                {
                    // 没有找到，添加剩余部分并退出
                    parts.emplace_back(sp.begin(), sp.end());
                    break;
                }
            }

            // 验证结果
            assert(parts.size() == 5);
            assert(parts[0] == "hello");
            assert(parts[1] == "/world");
            assert(parts[2] == "/a");
            assert(parts[3] == "/b");
            assert(parts[4] == "/c");
        }
        {
            std::string s = "hello/world/a/b/c";
            std::span<const char> sp{s};

            // 处理第一个非 '/' 开头的部分
            if (auto it = std::ranges::find(sp, '/'); it != sp.end())
            {
                std::cout << std::string(sp.begin(), it) << '\n'; // 输出 "hello"
                sp = sp.subspan(std::distance(sp.begin(), it));   // 剩余部分以 '/' 开头
            }

            // 循环处理后续的 "/str" 部分
            while (!sp.empty())
            {
                auto tail = sp.subspan(1); // 跳过当前 '/'
                if (auto next = std::ranges::find(tail, '/'); next != tail.end())
                {
                    // 计算当前 "/str" 的长度（包含末尾的 '/'）
                    size_t len = 1 + std::distance(tail.begin(), next);
                    std::cout << std::string(sp.begin(), sp.begin() + len) << '\n';
                    sp = sp.subspan(len); // 移动到下一段
                }
                else
                {
                    // 输出最后一段
                    std::cout << std::string(sp.begin(), sp.end()) << '\n';
                    break;
                }
            }
        }
        // find
        {
            std::string s = "hello/world/a/b/c";
            std::span<const char> sp{s};
            auto next = std::ranges::find(sp, '/');
            assert(next != sp.end());
            assert(sp[5] == '/');
            // NOTE: distance 可以代表 index
            assert(std::distance(sp.begin(), next) == 5);

            // NOTE: distance 可以直接算出
            assert(std::distance(sp.begin(), next) == next - sp.begin());

            // 带范围的查找
            assert(sp[11] == '/');
            auto next2 = std::ranges::find(next + 1, sp.end(), '/');
            assert(std::distance(sp.begin(), next2) == 11);

            auto l = (next2 - next);
            assert(std::string_view{"world/"}.length() == l);

            auto v = sp.subspan(std::distance(sp.begin(), next) + 1, l - 1);
            assert(std::string(v.begin(), v.end()) == "world");

            // 0
            v = sp.subspan(0, 0);
            assert(v.empty());
            assert(std::string(v.begin(), v.end()) == "");

            // NOTE: 未定义行为
            // v = sp.subspan(-1, 0);

            {
                std::string s = "//";
                std::span<const char> sp{s};
                auto next = std::ranges::find(sp, '/');
                assert(next != sp.end());
                assert(sp[0] == '/');

                auto next2 = std::ranges::find(sp.begin() + 1, sp.end(), '/');
                auto l = (next2 - next);
                assert(l == 1);
                auto check = sp.subspan(1, l - 1);
                assert(check.empty());

                // 迭代下一轮回
                sp = sp.subspan(l);
                assert(std::string(sp.begin(), sp.end()) == "/");
                assert(sp[0]);
                assert(sp.begin() + 1 == sp.end());
                // next = std::ranges::find(sp, '/');
            }
        }
        {
            std::string s = "hello/world/a/b/c";
            std::span<const char> sp{s};

            const size_t k_count = 5;
            const std::string_view k_str = "hello";

            auto sub = sp.subspan(0, k_count);
            assert(std::string(sub.begin(), sub.end()) == k_str);

            auto sub2 = sp.first(k_count);
            assert(std::string(sub2.begin(), sub2.end()) == k_str);

            // NOTE: subspan 和 first 都从 0 索引处，count一样，结果是一样的
            auto next = std::ranges::find(sp, '/');
            auto d = std::distance(sp.begin(), next);
            assert(d == k_count);
            // NOTE: sub.first(next) 恰好不包括 '/'
            assert(std::string(sp.begin(), next) == "hello");
            assert(std::string_view(sp.first(d)) == "hello");
            assert(std::string_view(sp.subspan(0, d)) == "hello");

            // NOTE: 但是 d 指向 '/'
            assert(sp[k_count] == '/');
            assert(sp[d] == '/');
        }
    }
    {
        // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
        {
            std::string s = "1.1.1.1";
            std::span<const char> sp{s};
            int idx1{1};
            int idx2{3};
            int idx3{5}; // NOLINT
            assert(sp[idx1] == '.' && sp[idx2] == '.' && sp[idx3] == '.');
            auto sub1 = sp.subspan(0, idx1);
            assert(std::string(sub1.begin(), sub1.end()) == "1");

            auto sub2 = sp.subspan(idx1 + 1, idx2 - idx1 - 1);
            assert(std::string(sub2.begin(), sub2.end()) == "1");

            auto sub3 = sp.subspan(idx2 + 1, idx3 - idx2 - 1);
            assert(std::string(sub3.begin(), sub3.end()) == "1");

            auto sub4 =
                idx3 + 1 > sp.size() ? std::span<const char>{} : sp.subspan(idx3 + 1);
            assert(std::string(sub4.begin(), sub4.end()) == "1");
        }
        {
            std::string s = "...";
            std::span<const char> sp{s};

            // 使用 size_type 而不是 int
            std::size_t idx1 = 0;
            std::size_t idx2 = 1;
            std::size_t idx3 = 2;

            assert(sp[idx1] == '.' && sp[idx2] == '.' && sp[idx3] == '.');

            assert(idx1 < idx2 && idx2 < idx3);

            // 这些子范围操作可以简化
            auto sub1 = sp.first(idx1); // 等价于 subspan(0, idx1)
            assert(sub1.empty());

            auto sub2 = sp.subspan(idx1 + 1, idx2 - idx1 - 1); // size_t 不会小于0
            assert(sub2.empty());

            auto sub3 = sp.subspan(idx2 + 1, idx3 - idx2 - 1);
            assert(sub3.empty());

            auto sub4 =
                (idx3 + 1 == sp.size()) ? std::span<const char>{} : sp.subspan(idx3 + 1);
            assert(sub4.empty());

            // idx3 + 1 可能溢出吗？
            {
                // const auto k_size = std::numeric_limits<size_t>::max() / 1000;
                const auto k_size = 10000;
                /**
                 * @brief terminate called after throwing an instance of
                 * 'std::length_error' what():  cannot create std::vector larger than
                 * max_size()
                 */
                std::vector<char> arr(k_size);
                std::span<const char> sp{arr};
                assert(arr.size() == k_size);

                // 查找的 索引不会越界
                assert(((arr.size() - 1) + 1) == arr.size());

                // NOTE: 如果扩容呢。 如果很大，直接崩溃，不用担心 index + 1 溢出
                auto i = k_size + 1;

                std::cout << " i : " << i << '\n';
            }
        }
    }
    {
        std::string s = "hello/world/a/b/c";
        std::span<const char> sp{s.data(), s.size()}; // 转换为 span

        auto [left, right] = split_first_span(sp, '/');

        // 输出结果
        std::cout << "\nstr: " << s << "\n";
        std::cout << "Left: " << std::string(left.begin(), left.end()) << "\n";
        std::cout << "Right: " << std::string(right.begin(), right.end()) << "\n";

        assert(std::string(left.begin(), left.end()) == "hello");
        assert(std::string(right.begin(), right.end()) == "world/a/b/c");

        {
            std::string s = "hello/";
            std::span<const char> sp{s.data(), s.size()}; // 转换为 span

            auto [left, right] = split_first_span(sp, '/');

            // 输出结果
            std::cout << "\nstr: " << s << "\n";
            std::cout << "Left: " << std::string(left.begin(), left.end()) << "\n";
            std::cout << "Right: " << std::string(right.begin(), right.end()) << "\n";

            assert(std::string(left.begin(), left.end()) == "hello");
            assert(std::string(right.begin(), right.end()) == "");
        }

        {
            std::string s = "/hello";
            std::span<const char> sp{s.data(), s.size()}; // 转换为 span

            auto [left, right] = split_first_span(sp, '/');

            // 输出结果
            std::cout << "\nstr: " << s << "\n";
            std::cout << "Left: " << std::string(left.begin(), left.end()) << "\n";
            std::cout << "Right: " << std::string(right.begin(), right.end()) << "\n";

            assert(std::string(left.begin(), left.end()) == "");
            assert(std::string(right.begin(), right.end()) == "hello");
        }

        {
            std::string s = "/";
            std::span<const char> sp{s.data(), s.size()}; // 转换为 span

            auto [left, right] = split_first_span(sp, '/');

            // 输出结果
            std::cout << "\nstr: " << s << "\n";
            std::cout << "Left: " << std::string(left.begin(), left.end()) << "\n";
            std::cout << "Right: " << std::string(right.begin(), right.end()) << "\n";

            assert(std::string(left.begin(), left.end()) == "");
            assert(std::string(right.begin(), right.end()) == "");
        }
        {
            std::string s;
            std::span<const char> sp{s.data(), s.size()}; // 转换为 span

            auto [left, right] = split_first_span(sp, '/');

            // 输出结果
            std::cout << "\nstr: " << s << "\n";
            std::cout << "Left: " << std::string(left.begin(), left.end()) << "\n";
            std::cout << "Right: " << std::string(right.begin(), right.end()) << "\n";

            assert(std::string(left.begin(), left.end()) == "");
            assert(std::string(right.begin(), right.end()) == "");
        }
        {
            std::string s = "abc";
            std::span<const char> sp{s.data(), s.size()}; // 转换为 span

            auto [left, right] = split_first_span(sp, '/');

            // 输出结果
            std::cout << "\nstr: " << s << "\n";
            std::cout << "Left: " << std::string(left.begin(), left.end()) << "\n";
            std::cout << "Right: " << std::string(right.begin(), right.end()) << "\n";

            assert(std::string(left.begin(), left.end()) == "abc");
            assert(std::string(right.begin(), right.end()) == "");
        }
    }
    std::cout << "main done\n";
    return 0;
}