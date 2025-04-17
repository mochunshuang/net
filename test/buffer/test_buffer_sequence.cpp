
#include <cassert>
#include <cstddef> // For std::byte
#include <vector>

// NOLINTBEGIN
#include "../test_head.hpp"

using namespace mcs::net::buffer; // NOLINT

// --------------------- 测试代码 ---------------------

// Test 1: 单个 mutable_buffer 的序列范围
void test_single_mutable_buffer_sequence()
{
    mutable_buffer buf;
    const auto *begin = mcs::net::buffer::buffer_sequence_begin(buf);
    const auto *end = mcs::net::buffer::buffer_sequence_end(buf);

    REQUIRE_TRUE(begin == &buf);    // 起始位置指向 buf 的地址
    REQUIRE_TRUE(end == &buf + 1);  // 结束位置为起始地址 +1
    REQUIRE_TRUE(end - begin == 1); // 序列长度为 1
}

// Test 2: 单个 const_buffer 的序列范围
void test_single_const_buffer_sequence()
{
    const const_buffer buf;
    const auto *begin = mcs::net::buffer::buffer_sequence_begin(buf);
    const auto *end = mcs::net::buffer::buffer_sequence_end(buf);

    REQUIRE_TRUE(begin == &buf);
    REQUIRE_TRUE(end == &buf + 1);
    REQUIRE_TRUE(end - begin == 1);
}

// Test 3: 泛型容器（std::vector<mutable_buffer>）
void test_vector_container()
{

    std::vector<mutable_buffer> vec(3);
    auto begin = mcs::net::buffer::buffer_sequence_begin(vec);
    auto end = mcs::net::buffer::buffer_sequence_end(vec);

    REQUIRE_TRUE(begin == vec.begin()); // 迭代器指向容器的起始
    REQUIRE_TRUE(end == vec.end());     // 迭代器指向容器的末尾
    REQUIRE_TRUE(end - begin == 3);     // 容器包含 3 个元素
}

// Test 4: 泛型容器（std::array<const_buffer, 2>）
void test_array_container()
{
    std::array<const_buffer, 2> arr{};
    auto begin = mcs::net::buffer::buffer_sequence_begin(arr);
    auto end = mcs::net::buffer::buffer_sequence_end(arr);

    REQUIRE_TRUE(begin == arr.begin());
    REQUIRE_TRUE(end == arr.end());
    REQUIRE_TRUE(end - begin == 2);
}

// Test 5: 空容器的处理
void test_empty_container()
{
    std::vector<mutable_buffer> empty_vec;
    auto begin = mcs::net::buffer::buffer_sequence_begin(empty_vec);
    auto end = mcs::net::buffer::buffer_sequence_end(empty_vec);

    REQUIRE_TRUE(begin == empty_vec.begin());
    REQUIRE_TRUE(end == empty_vec.end());
    REQUIRE_TRUE(begin == end); // 空容器的 begin 等于 end
}

// Test 6: const 容器的迭代器
void test_const_container()
{
    const std::vector<const_buffer> cvec(2);
    auto begin = mcs::net::buffer::buffer_sequence_begin(cvec);
    auto end = mcs::net::buffer::buffer_sequence_end(cvec);

    REQUIRE_TRUE(begin == cvec.begin()); // 正确获取 const 容器的 const_iterator
    REQUIRE_TRUE(end == cvec.end());
    REQUIRE_TRUE(end - begin == 2);
}

// Test 7: 混合类型容器（如 mutable_buffer 和 const_buffer 的混合）
void test_mixed_container()
{
    std::vector<const_buffer> mixed_vec;
    mixed_vec.push_back(const_buffer{});                 // 添加 const_buffer
    mixed_vec.push_back(const_buffer{mutable_buffer{}}); // 从 mutable_buffer 转换

    auto begin = mcs::net::buffer::buffer_sequence_begin(mixed_vec);
    auto end = mcs::net::buffer::buffer_sequence_end(mixed_vec);
    REQUIRE_TRUE(end - begin == 2);
}

// Test 8: 验证返回类型是否符合预期
void test_return_types()
{
    mutable_buffer buf;
    std::vector<mutable_buffer> vec;

    // 验证 mutable_buffer 的重载返回 const mutable_buffer*
    static_assert(std::is_same_v<decltype(mcs::net::buffer::buffer_sequence_begin(buf)),
                                 const mutable_buffer *>);

    // 验证容器的重载返回迭代器类型
    static_assert(std::is_same_v<decltype(mcs::net::buffer::buffer_sequence_begin(vec)),
                                 std::vector<mutable_buffer>::iterator>);

    // 验证 const 容器的重载返回 const_iterator
    const std::vector<mutable_buffer> cvec;
    static_assert(std::is_same_v<decltype(mcs::net::buffer::buffer_sequence_begin(cvec)),
                                 std::vector<mutable_buffer>::const_iterator>);
}

int main()
{
    test_single_mutable_buffer_sequence();
    test_single_const_buffer_sequence();
    test_vector_container();
    test_array_container();
    test_empty_container();
    test_const_container();
    test_mixed_container();
    test_return_types();

    return 0;
}
// NOLINTEND