#include <span>
#include <cstdint>
#include <utility>
#include <iostream>
#include <cassert>

using octet_t = std::uint8_t;
using default_span_t = std::span<const char>;
constexpr auto empty_span = default_span_t{};

constexpr auto split_span_last(const default_span_t &sp, const octet_t &c) noexcept
{
    for (size_t i = sp.size(); i-- > 0;)
    {
        if (sp[i] == c)
        {
            return std::pair{sp.first(i), sp.subspan(i + 1)};
        }
    }
    return std::pair{empty_span, sp};
}

#include <iostream>

int main()
{
    {
        std::string s = "hello/world/a/b/c";
        std::span<const char> sp{s.data(), s.size()};

        auto [left, right] = split_span_last(sp, '/');

        assert(std::string(left.begin(), left.end()) == "hello/world/a/b");
        assert(std::string(right.begin(), right.end()) == "c");
    }

    {
        std::string s = "hello/";
        std::span<const char> sp{s.data(), s.size()};

        auto [left, right] = split_span_last(sp, '/');

        assert(std::string(left.begin(), left.end()) == "hello");
        assert(std::string(right.begin(), right.end()) == "");
    }

    {
        std::string s = "/hello";
        std::span<const char> sp{s.data(), s.size()};

        auto [left, right] = split_span_last(sp, '/');

        assert(std::string(left.begin(), left.end()) == "");
        assert(std::string(right.begin(), right.end()) == "hello");
    }

    {
        std::string s = "/";
        std::span<const char> sp{s.data(), s.size()};

        auto [left, right] = split_span_last(sp, '/');

        assert(std::string(left.begin(), left.end()) == "");
        assert(std::string(right.begin(), right.end()) == "");
    }

    {
        std::string s;
        std::span<const char> sp{s.data(), s.size()};

        auto [left, right] = split_span_last(sp, '/');

        assert(left.empty());
        assert(right.empty());
    }

    {
        std::string s = "abc";
        std::span<const char> sp{s.data(), s.size()};

        auto [left, right] = split_span_last(sp, '/');

        assert(left.empty());
        assert(std::string(right.begin(), right.end()) == "abc");
    }
    std::cout << "main done\n";
    return 0;
}