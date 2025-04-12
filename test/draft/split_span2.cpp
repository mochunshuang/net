#include <span>
#include <cstdint>
#include <utility>
#include <iostream>
#include <cassert>

#include <vector>
#include <array>

using octet_t = std::uint8_t;
using default_span_t = std::span<const char>;
constexpr auto empty_span = default_span_t{};

constexpr auto split_span_last(default_span_t sp, octet_t c) noexcept
{
    const auto k_size = sp.size();
    for (size_t i = k_size; i-- > 0;)
    {
        // sp.subspan(k_size) 是合法的，会返回空
        // sp.subspan:
        //  return _ReturnType{_Mydata + _Offset, _Count == dynamic_extent ? _Mysize -
        //  _Offset : _Count};
        if (sp[i] == c)
            return std::pair{sp.first(i),
                             i + 1 == k_size ? empty_span : sp.subspan(i + 1)};
    }
    return std::pair{empty_span, sp}; // not find c in sp
}

#include <iostream>

int main()
{
    {
        for (size_t i = 1; i-- > 0;)
        {
            assert(i == 0);
        }
        for (size_t i = 1; --i > 0;)
        {
            assert(false);
        }
        std::string s = "hello/world/a/b/c";
        std::span<const char> sp{s.data(), s.size()};
        std::cout << "size: " << s.length() << '\n';
        assert(s.length() == 17);
        assert(sp.subspan(s.length()).empty()); // 是合法的

        // NOTE: 崩溃
        assert(sp.subspan(17).empty()); // 为 size 是合法的
        // assert(sp.subspan(1, 16).empty());  //崩溃
        // assert(sp.subspan(18).empty()); //崩溃

        for (int i = 100000; i-- > 0;) // NOLINT
        {
            assert(sp.subspan(17).empty());
        }
        // NOTE: 没有崩溃？
        // auto ss = sp.subspan(s.length() + 1);
        // Offset <= size() && (Count == std::dynamic_extent || Count <= size() - Offset)
        // 1 <= 1 && ( 1 <= 1 - 1) ==> true && false == false 等于未定义
        // sp.size()==1时,sp.subspan(1,1) //NOTE: 未定义，还是得自己判断的
        // sp.subspan(size,1)
        // NOTE: 要求：
        // _Offset <= _Mysize && _Count == dynamic_extent || _Count <= _Mysize - _Offset
        // NOTE: 推论：
        // _Offset <= _Mysize && _Count == dynamic_extent || _Count +  _Offset<= _Mysize
        // NOTE: 推论： _Offset <= _Mysize 且 只要不设置 _Count 就是OK的
        /**
         _STL_VERIFY(_Offset <= _Mysize, "Offset out of range in span::subspan(offset,
        count)");
        _STL_VERIFY(_Count == dynamic_extent || _Count <= _Mysize - _Offset,
            "Count out of range in span::subspan(offset, count)");
         */
    }
    {
        auto print = [](std::string_view const name, std::size_t ex) {
            std::cout << name << ", ";
            if (std::dynamic_extent == ex)
                std::cout << "动态尺度\n";
            else
                std::cout << "静态尺度 = " << ex << '\n';
        };

        int a[]{1, 2, 3, 4, 5};

        std::span span1{a};
        print("span1", span1.extent);
        {
            // constexpr int a[]{1, 2, 3, 4, 5};
            // constexpr std::span span1{a}; //失败
        }

        std::span<int, std::dynamic_extent> span2{a};
        print("span2", span2.extent);

        std::array ar{1, 2, 3, 4, 5};
        std::span span3{ar};
        print("span3", span3.extent);

        std::vector v{1, 2, 3, 4, 5};
        std::span span4{v};
        print("span4", span4.extent);
    }
    {
        std::string s = "//hello/world/a/b/c";
        std::span<const char> sp{s.data(), s.size()};
        assert(sp[7] == '/');
        auto sub0 = sp.subspan(2, 7 - 2); // NOLINT
        auto sub1 = sp.subspan(7);        // NOLINT
        assert(std::string(sub0.begin(), sub0.end()) == "hello");
        assert(std::string(sub1.begin(), sub1.end()) == "/world/a/b/c");
    }
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