
#include "./test_head.hpp"

#include <cassert>
#include <iostream>

int main()
{
    using namespace mcs::abnf::__detail; // NOLINT

    TEST("test_spans") = [] {
        // 创建测试用的 span 对象
        span s1{.start = 1, .end = 2};
        span s2{.start = 3, .end = 4};
        span s3{.start = 5, .end = 6}; // NOLINT

        // 测试 spans 构造和 operator[]
        spans<3> fs{s1, s2, s3};
        assert(fs[0].start == 1 && fs[0].end == 2);
        assert(fs[1].start == 3 && fs[1].end == 4);
        assert(fs[2].start == 5 && fs[2].end == 6);

        // 测试结构化绑定 (C++17 起支持)
        auto &[a, b, c] = fs;
        assert(a.start == 1 && a.end == 2);
        assert(b.start == 3 && b.end == 4);
        assert(c.start == 5 && c.end == 6);

        // 测试成员函数 get
        static_assert(fs.size() == 3);
        assert(fs.get<0>().start == 1);
        assert(fs.get<1>().end == 4);
        assert(fs.get<2>().start == 5);

        assert(fs.get<0>() == std::get<0>(fs));
        assert(fs.get<1>().end == 4);
        assert(fs.get<2>().start == 5);

        // // 测试 tuple_size 和 tuple_element 特化
        static_assert(std::tuple_size_v<decltype(fs)> == 3);
        static_assert(std::is_same_v<std::tuple_element_t<0, decltype(fs)>, span>);
    };
    std::cout << "main done\n";
    return 0;
}